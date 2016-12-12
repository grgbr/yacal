#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "string.h"
#include "pager.h"

struct pager_atom {
	char const*           pg_str;
	size_t                pg_len;
	pager_render_atom_fn* pg_render;
};

#define pg_assert_atom(_atom)     \
	ut_assert(_atom);         \
	ut_assert(_atom->pg_str); \
	ut_assert(_atom->pg_len); \
	ut_assert(_atom->pg_render)

struct pager_line {
	char*        pg_str;
	unsigned int pg_indent;
	unsigned int pg_atms_cnt;
	unsigned int pg_first_atm;
	size_t       pg_len;
};

#define pg_assert_ln(_line) \
	ut_assert(_line)

static __nonull(1)
int pg_paint_atm(struct pager_atom const* thiz, void* data)
{
	pg_assert_atom(thiz);

	return thiz->pg_render(thiz->pg_str, thiz->pg_len, data);
}

static __nonull(1, 2, 4)
int pg_paint_ln(struct pager_line const* thiz,
                struct vector const*     atoms,
                unsigned int             line_no,
                pager_render_line_fn*    render_line,
                void*                    data)
{
	int          err;
	unsigned int a;
	unsigned int cnt = thiz->pg_first_atm + thiz->pg_atms_cnt;

	err = (*render_line)(thiz->pg_str,
	                     thiz->pg_len,
	                     line_no,
	                     thiz->pg_indent,
	                     data);
	if (err)
		return err;

	for (a = thiz->pg_first_atm; a < cnt; a++) {
		err = pg_paint_atm((struct pager_atom*)vect_entry(atoms, a),
		                   data);
		if (err)
			return err;
	}

	return 0;
}

static __nonull(1) __nothrow __pure
struct pager_line* pg_last_ln(struct vector const* lines)
{
	ut_assert(lines);

	return (struct pager_line*)vect_entry(lines, vect_count(lines) - 1);
}

static __nonull(1, 2) __nothrow
void pg_complete_ln(struct pager_line*   line,
                    struct vector const* atoms)
{
	struct pager_atom* atm;
	size_t             blanks;

	if (!line->pg_atms_cnt)
		return;

	atm = (struct pager_atom*)
	      vect_entry(atoms,
	                 line->pg_first_atm + line->pg_atms_cnt - 1);

	blanks = str_count_trailing_blank(atm->pg_str, atm->pg_len);
	if (!blanks)
		return;

	atm->pg_len -= blanks;
	line->pg_len -= blanks;
}

static __nonull(1) __nothrow
void pg_stop_feed(struct pager* thiz)
{
	pg_assert(thiz);

	if (thiz->pg_stat != PG_INIT_STAT) {
		unsigned int         l;
		struct vector* const lns = &thiz->pg_lns;

		if (thiz->pg_stat == PG_FILL_STAT)
			pg_complete_ln(pg_last_ln(lns), &thiz->pg_atms);

		for (l = 0; l < vect_count(lns); l++) {
			struct pager_line* ln;

			ln = (struct pager_line*)vect_entry(lns, l);
			pg_assert_ln(ln);

			if (ln->pg_len)
				thiz->pg_max_len = ut_max(thiz->pg_max_len,
				                          ln->pg_indent +
				                          ln->pg_len);
		}

		vect_shrink(lns);
		vect_shrink(&thiz->pg_atms);

		thiz->pg_stat = PG_INIT_STAT;
	}
}

__nonull(1)
int pg_paint(struct pager* thiz,
             unsigned int  top_line,
             unsigned int  row_nr,
             void*         data)
{
	pg_assert(thiz);

	struct vector const* const lns = &thiz->pg_lns;
	unsigned int               cnt;
	struct vector const* const atms = &thiz->pg_atms;
	int                        err;
	unsigned int               r;

	pg_stop_feed(thiz);

	cnt = vect_count(lns);
	if (top_line >= cnt)
		return -ERANGE;

	err = pg_paint_ln((struct pager_line*)vect_entry(lns, top_line),
	                  atms,
	                  0,
	                  thiz->pg_render,
	                  data);
	if (err)
		return err;

	row_nr = ut_min(row_nr, cnt - top_line);
	ut_assert(row_nr);

	for (r = 1; r < row_nr; r++) {
		err = pg_paint_ln((struct pager_line*)vect_entry(lns,
		                                                 top_line + r),
		                  atms,
		                  r,
		                  thiz->pg_render,
		                  data);
		if (err)
			return err;
	}

	return 0;
}

static __nonull(1, 2) __nothrow
int pg_push_ln(struct vector*       lines,
               struct vector const* atoms,
               unsigned int         indent)
{
	ut_assert(lines);
	ut_assert(atoms);

	struct pager_line* ln;

	ln = (struct pager_line*)vect_push(lines);
	if (!ln)
		return -errno;

	ln->pg_str = NULL;
	ln->pg_indent = indent;
	ln->pg_atms_cnt = 0;
	ln->pg_first_atm = vect_count(atoms);
	ln->pg_len = 0;

	return 0;
}

static __nonull(1, 2, 3, 4, 6) __nothrow
int pg_push_atm(struct pager_line*    line,
                struct vector*        atoms,
                char* restrict        line_string,
                char const* restrict  orig_string,
		size_t                length,
                pager_render_atom_fn* render)
{
	pg_assert_ln(line);
	ut_assert(atoms);
	ut_assert(line_string);
	ut_assert(orig_string && (orig_string[0] != '\0'));
	ut_assert(length);
	ut_assert(render);

	struct pager_atom* atm;
	size_t             len;

	atm = (struct pager_atom*)vect_push(atoms);
	if (!atm)
		return -errno;

	len = str_squeeze_blanks(line_string, orig_string, length);
	line_string[len] = '\0';

	atm->pg_str = line_string;
	atm->pg_len = len;
	atm->pg_render = render;

	line->pg_atms_cnt++;
	line->pg_len += len;

	return 0;
}

static __nonull(1, 2) __nothrow
ssize_t pg_init_parse(struct pager* thiz, char const* string)
{
	pg_assert(thiz);
	ut_assert(string && (string[0] != '\0'));

	size_t           indent;
	enum pager_state stat;
	ssize_t          len;
	int              err;

	if (*string == '\n') {
		indent = 0;
		stat = PG_SCAN_STAT;
		len = 1;
	}
	else if (isblank(*string)) {
		indent = 1 + strspn(string + 1, " \t");
		stat = PG_SCAN_STAT;
		len = indent;
	}
	else {
		indent = 0;
		stat = PG_FILL_STAT;
		len = 0;
	}

	err = pg_push_ln(&thiz->pg_lns, &thiz->pg_atms, indent);
	if (err)
		return err;

	thiz->pg_stat = stat;

	return len;
}

static __nonull(1, 3) __nothrow
size_t pg_find_farthest_word(char const* string,
                             size_t      length,
                             size_t*     available,
                             bool        empty)
{
	size_t wlen;
	size_t dlen;
	size_t len = 0;

	wlen = str_count_leading_noalnum(string, length);
	if (wlen) {
		dlen = wlen - str_count_blank_dups(string, wlen);
		if (dlen > *available)
			goto out;

		*available -= dlen;
		len += wlen;
		length -= wlen;
	}

	while (length) {
		wlen = str_count_leading_alnum(&string[len], length);
		wlen += str_count_leading_noalnum(&string[len + wlen],
		                                  length - wlen);

		dlen = wlen - str_count_blank_dups(&string[len], wlen);
		if (dlen > *available)
			break;

		*available -= dlen;
		len += wlen;
		length -= wlen;
	}

out:
	if (!empty || len)
		return len;

	return str_eval_squeeze_blanks(string, length, available);
}

static __nonull(1, 2, 3) __nothrow
ssize_t pg_fill_ln(struct pager*         thiz,
                   char const*           string,
                   pager_render_atom_fn* render)
{
	pg_assert(thiz);
	ut_assert(string && (string[0] != '\0'));
	ut_assert(render);

	int                  err;
	struct vector* const lns = &thiz->pg_lns;
	struct vector* const atms = &thiz->pg_atms;
	struct pager_line*   ln;
	size_t               len;
	size_t               avail;
	char*                lnstr;

	ln = pg_last_ln(lns);

	if (*string == '\n') {
		pg_complete_ln(ln, atms);

		err = pg_push_ln(lns, atms, 0);
		if (err)
			return err;

		thiz->pg_stat = PG_SCAN_STAT;

		return 1;
	}

	len = strchrnul(string, '\n') - string;
	ut_assert(len);

	avail = thiz->pg_sz - 1 - ln->pg_len;
	len = pg_find_farthest_word(string, len, &avail, !ln->pg_atms_cnt);
	if (!len)
		goto newln;

	if (!ln->pg_atms_cnt) {
		ut_assert(!isblank(string[0]));
		ut_assert(!ln->pg_len);

		ln->pg_str = malloc(thiz->pg_sz);
		if (!ln->pg_str)
			return -errno;

		lnstr = ln->pg_str;
	}
	else {
		lnstr = &ln->pg_str[ln->pg_len];
		lnstr -= (size_t)(isblank(string[0]) && isblank(*(lnstr - 1)));
	}

	err = pg_push_atm(ln, atms, lnstr, string, len, render);
	if (err)
		return err;

	if (avail)
		/* Line is not yet full. */
		return len;

newln:
	pg_complete_ln(ln, atms);

	thiz->pg_stat = PG_BREAK_STAT;

	return len;
}

static __nonull(1, 2) __nothrow
ssize_t pg_scan_str(struct pager* thiz, char const* string)
{
	pg_assert(thiz);
	ut_assert(string && (string[0] != '\0'));

	int                  err;
	struct vector* const lns = &thiz->pg_lns;
	struct vector* const atms = &thiz->pg_atms;

	if (*string == '\n') {
		err = pg_push_ln(lns, atms, 0);
		if (err)
			return err;

		return 1;
	}

	if (isblank(*string)) {
		size_t indent;

		indent = 1 + strspn(string + 1, " \t");
		pg_last_ln(lns)->pg_indent += indent;

		return (ssize_t)indent;
	}

	thiz->pg_stat = PG_FILL_STAT;

	return 0;
}

static __nonull(1, 2) __nothrow
ssize_t pg_break_ln(struct pager* thiz, char const* string)
{
	pg_assert(thiz);
	ut_assert(string && (string[0] != '\0'));

	int                  err;
	struct vector* const lns = &thiz->pg_lns;
	struct vector* const atms = &thiz->pg_atms;

	if (*string == '\n') {
		err = pg_push_ln(lns, atms, 0);
		if (err)
			return err;

		thiz->pg_stat = PG_SCAN_STAT;

		return 1;
	}

	if (isblank(*string))
		return 1 + strspn(string + 1, " \t");

	err = pg_push_ln(lns, atms, pg_last_ln(lns)->pg_indent);
	if (err)
		return err;

	thiz->pg_stat = PG_FILL_STAT;

	return 0;
}

__nonull(1, 2, 3) __nothrow
int pg_feed_string(struct pager*         thiz,
                   char const*           string,
                   pager_render_atom_fn* render)
{
	pg_assert(thiz);
	ut_assert(string && (string[0] != '\0'));
	ut_assert(render);

	do {
		ssize_t len;

		switch (thiz->pg_stat) {
		case PG_INIT_STAT:
			len = pg_init_parse(thiz, string);
			break;

		case PG_SCAN_STAT:
			len = pg_scan_str(thiz, string);
			break;

		case PG_FILL_STAT:
			len = pg_fill_ln(thiz, string, render);
			break;

		case PG_BREAK_STAT:
			len = pg_break_ln(thiz, string);
			break;

		default:
			ut_assert(0);
		}

		if (len < 0)
			return len;

		string += len;
	} while (*string != '\0');

	return 0;
}

__nonull(1) __nothrow
int pg_feed_line(struct pager* thiz, unsigned int indent)
{
	pg_assert(thiz);

	unsigned int               cnt;
	struct vector* const       lns = &thiz->pg_lns;
	struct vector const* const atms = &thiz->pg_atms;

	cnt = vect_count(lns);
	if (cnt)
		pg_complete_ln((struct pager_line*)vect_entry(lns, cnt - 1),
		               atms);

	thiz->pg_stat = PG_SCAN_STAT;

	return pg_push_ln(lns, atms, indent);
}

static __nonull(1) __nothrow
void pg_free_lns(struct vector const* lines)
{
	ut_assert(lines);

	unsigned int l;

	for (l = 0; l < vect_count(lines); l++) {
		struct pager_line* ln;

		ln = (struct pager_line*)vect_entry(lines, l);
		pg_assert_ln(ln);

		if (ln->pg_atms_cnt)
			free(ln->pg_str);
	}
}

__nonull(1) __nothrow
void pg_clear(struct pager* thiz)
{
	pg_assert(thiz);

	struct vector* const lns = &thiz->pg_lns;

	if (vect_count(lns))
		pg_free_lns(lns);

	vect_clear(&thiz->pg_atms);
	vect_clear(lns);

	thiz->pg_stat = PG_INIT_STAT;
}

__nonull(1, 3) __nothrow
bool pg_dress(struct pager*         thiz,
              size_t                row_length,
              pager_render_line_fn* render_line)
{
	pg_assert(thiz);
	ut_assert(row_length);
	ut_assert(render_line);

	struct vector* const lns = &thiz->pg_lns;

	if (vect_count(lns) && (++row_length == thiz->pg_sz))
		/* Pager was fed and no more rendering needed : just return. */
		return true;

	pg_clear(thiz);

	thiz->pg_render = render_line;
	thiz->pg_max_len = 0;
	thiz->pg_sz = row_length;

	return false;
}

__nonull(1) __nothrow
int pg_init(struct pager* thiz)
{
	ut_assert(thiz);

	int err;

	err = vect_init(&thiz->pg_atms, sizeof(struct pager_atom));
	if (err)
		return err;

	err = vect_init(&thiz->pg_lns, sizeof(struct pager_line));
	if (err) {
		vect_fini(&thiz->pg_atms);
		return err;
	}

	thiz->pg_stat = PG_INIT_STAT;

	return 0;
}

__nonull(1) __nothrow
void pg_fini(struct pager* thiz)
{
	pg_assert(thiz);

	pg_free_lns(&thiz->pg_lns);
	vect_fini(&thiz->pg_lns);
	vect_fini(&thiz->pg_atms);
}
