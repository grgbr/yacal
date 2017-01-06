#include <stdio.h>
#include <errno.h>
#include <check.h>
#include "pager.h"

struct uc_pager_line {
	char*        str;
	size_t       len;
	unsigned int indent;
};

struct uc_pager_atom {
	char*  str;
	size_t len;
};

#define UC_PAGER_LINE_NR (8U)

struct uc_pager_data {
	unsigned int         ln_nr;
	struct uc_pager_line exp_ln[UC_PAGER_LINE_NR + 1];
	struct uc_pager_atom exp_atm[4 * UC_PAGER_LINE_NR];
};

static unsigned int uc_pager_atom_no;

static struct pager         uc_pager;
static struct uc_pager_data uc_data;

static int uc_render_pager_atom(char const*  string,
                                size_t       length,
                                void*        data)
{
	struct uc_pager_data* const d = data;

	ck_assert_ptr_eq(data, &uc_data);
	ck_assert_uint_eq(length, d->exp_atm[uc_pager_atom_no].len);
	if (!length) {
		ck_assert_ptr_eq(string, NULL);
		ck_assert_ptr_eq(d->exp_atm[uc_pager_atom_no].str, NULL);
	}
	else
		ck_assert_int_eq(strncmp(string,
		                         d->exp_atm[uc_pager_atom_no].str,
		                         length),
		                 0);

	uc_pager_atom_no++;

	return 0;
}

static int uc_render_pager_line(char const*  string,
                                size_t       length,
                                unsigned int line_no,
                                unsigned int indent,
                                void*        data)
{
	struct uc_pager_data* const d = data;

	ck_assert_ptr_eq(data, &uc_data);
	ck_assert_uint_eq(line_no, d->ln_nr++);
	ck_assert_uint_eq(length, d->exp_ln[line_no].len);
	if (!length) {
		ck_assert_ptr_eq(string, NULL);
		ck_assert_ptr_eq(d->exp_ln[line_no].str, NULL);
	}
	else
		ck_assert_int_eq(strncmp(string,
		                         d->exp_ln[line_no].str,
		                         length),
		                 0);
	ck_assert_uint_eq(indent, d->exp_ln[line_no].indent);

	return 0;
}

START_TEST(uc_pager_life_cycle_start_feed)
{
	ck_assert_uint_eq(pg_max_len(&uc_pager), 0);
	ck_assert_uint_eq(pg_line_count(&uc_pager), 0);
}
END_TEST

START_TEST(uc_pager_life_cycle_paint)
{
	ck_assert_int_eq(pg_paint(&uc_pager, 0, 10, &uc_data), -ERANGE);

	ck_assert_uint_eq(pg_max_len(&uc_pager), 0);
	ck_assert_uint_eq(pg_line_count(&uc_pager), 0);
}
END_TEST

START_TEST(uc_pager_life_cycle_feed_line)
{
	ck_assert_int_eq(pg_feed_line(&uc_pager, 0), 0);

	ck_assert_uint_eq(pg_max_len(&uc_pager), 0);
	ck_assert_uint_eq(pg_line_count(&uc_pager), 1);

	uc_data.exp_ln[0].str = NULL;
	uc_data.exp_ln[0].len = 0;
	uc_data.exp_ln[0].indent = 0;

	ck_assert_int_eq(pg_paint(&uc_pager, 0, 10, &uc_data), 0);

	ck_assert_uint_eq(uc_data.ln_nr, 1);
	ck_assert_uint_eq(pg_max_len(&uc_pager), 0);
	ck_assert_uint_eq(pg_line_count(&uc_pager), 1);
}
END_TEST

START_TEST(uc_pager_life_cycle_feed_string)
{
	char* const  str = "string";
	size_t const len = strlen(str);

	ck_assert_int_eq(pg_feed_string(&uc_pager, str, uc_render_pager_atom),
	                 0);

	ck_assert_uint_eq(pg_max_len(&uc_pager), 0);
	ck_assert_uint_eq(pg_line_count(&uc_pager), 1);

	uc_data.exp_ln[0].str = str;
	uc_data.exp_ln[0].len = len;
	uc_data.exp_ln[0].indent = 0;

	uc_data.exp_atm[0].str = str;
	uc_data.exp_atm[0].len = len;

	ck_assert_int_eq(pg_paint(&uc_pager, 0, 10, &uc_data), 0);

	ck_assert_uint_eq(uc_data.ln_nr, 1);
	ck_assert_uint_eq(pg_max_len(&uc_pager), len);
	ck_assert_uint_eq(pg_line_count(&uc_pager), 1);
}
END_TEST

static void uc_pager_setup(void)
{
	memset(&uc_data, 0, sizeof(uc_data));
	uc_pager_atom_no = 0;

	ck_assert_int_eq(pg_init(&uc_pager), 0);

	pg_dress(&uc_pager, 16, uc_render_pager_line);
}

static void uc_pager_teardown(void)
{
	pg_fini(&uc_pager);
}

static void uc_pager_life_cycle_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Pager life cycle");
	tcase_add_checked_fixture(tc,
	                          uc_pager_setup,
	                          uc_pager_teardown);

	tcase_add_test(tc, uc_pager_life_cycle_start_feed);
	tcase_add_test(tc, uc_pager_life_cycle_paint);
	tcase_add_test(tc, uc_pager_life_cycle_feed_line);
	tcase_add_test(tc, uc_pager_life_cycle_feed_string);
	suite_add_tcase(suite, tc);
}

static void uc_fill_pager_line_data(char const*  string,
                                    char*        expect_string,
                                    size_t       expect_length,
                                    unsigned int indent,
                                    unsigned int line_no,
                                    unsigned int line_nr)
{
	assert(line_nr);

	ck_assert_int_eq(pg_feed_string(&uc_pager,
	                                string,
	                                uc_render_pager_atom),
	                 0);

	ck_assert_uint_eq(pg_max_len(&uc_pager), 0);
	ck_assert_uint_eq(pg_line_count(&uc_pager), line_nr);

	uc_data.exp_ln[line_no].str = expect_string;
	uc_data.exp_ln[line_no].len = expect_length;
	uc_data.exp_ln[line_no].indent = indent;
}

static void uc_fill_pager_atom_data(char*  expect_string,
                                    size_t expect_length)
{
	uc_data.exp_atm[uc_pager_atom_no].str = expect_string;
	uc_data.exp_atm[uc_pager_atom_no].len = expect_length;

	uc_pager_atom_no++;
}

static void uc_check_paint(unsigned int line_nr, unsigned int max_length)
{
	uc_pager_atom_no = 0;
	ck_assert_int_eq(pg_paint(&uc_pager,
	                          0,
	                          UC_PAGER_LINE_NR + 1,
	                          &uc_data),
	                 0);

	ck_assert_uint_eq(uc_data.ln_nr, line_nr);
	ck_assert_uint_eq(pg_max_len(&uc_pager), max_length);
	ck_assert_uint_eq(pg_line_count(&uc_pager), line_nr);
}

START_TEST(uc_pager_nobreak_multi_line)
{
	unsigned int s;
	size_t       len;

	for (s = 0; s < UC_PAGER_LINE_NR; s++) {
		char*  str;

		len = asprintf(&str, "string %d", s);

		ck_assert_int_eq(pg_feed_line(&uc_pager, 0), 0);
		uc_fill_pager_line_data(str, str, len, 0, s, s + 1);
		uc_fill_pager_atom_data(str, len);
	}

	uc_check_paint(UC_PAGER_LINE_NR, len);

	for (s = 0; s < UC_PAGER_LINE_NR; s++)
		free(uc_data.exp_ln[s].str);
}
END_TEST

START_TEST(uc_pager_nobreak_prenl)
{
	unsigned int s;
	size_t       len;

	for (s = 0; s < UC_PAGER_LINE_NR; s++) {
		char*  str;

		len = asprintf(&str, "\nstring %d", s);

		uc_fill_pager_line_data(str, str + 1, len - 1, 0, s, s + 1);
		uc_fill_pager_atom_data(str + 1, len - 1);
	}

	uc_check_paint(UC_PAGER_LINE_NR, len - 1);

	for (s = 0; s < UC_PAGER_LINE_NR; s++)
		free(uc_data.exp_ln[s].str - 1);
}
END_TEST

START_TEST(uc_pager_nobreak_postnl)
{
	unsigned int s;
	size_t       len;

	for (s = 0; s < UC_PAGER_LINE_NR; s++) {
		char*  str;

		len = asprintf(&str, "string %d\n", s);

		uc_fill_pager_line_data(str, str, len - 1, 0, s, s + 2);
		uc_fill_pager_atom_data(str, len - 1);
	}

	uc_check_paint(UC_PAGER_LINE_NR + 1, len - 1);

	for (s = 0; s < UC_PAGER_LINE_NR; s++)
		free(uc_data.exp_ln[s].str);
}
END_TEST

START_TEST(uc_pager_nobreak_multi_atom_nl)
{
	unsigned int s;

	for (s = 0; s < UC_PAGER_LINE_NR; s += 2) {
		char*  str;

		ck_assert_int_gt(asprintf(&str,
		                           "string %d\nstring %d",
		                           s,
		                           s + 1),
		                 0);

		ck_assert_int_eq(pg_feed_line(&uc_pager, 0), 0);

		ck_assert_int_eq(pg_feed_string(&uc_pager,
		                                str,
		                                uc_render_pager_atom),
		                 0);
		ck_assert_uint_eq(pg_max_len(&uc_pager), 0);
		ck_assert_uint_eq(pg_line_count(&uc_pager), s + 2);

		uc_data.exp_ln[s].str = str;
		uc_data.exp_ln[s].len = 8;
		uc_data.exp_ln[s].indent = 0;

		uc_data.exp_ln[s + 1].str = str + 9;
		uc_data.exp_ln[s + 1].len = 8;
		uc_data.exp_ln[s + 1].indent = 0;

		uc_fill_pager_atom_data(str, 8);
		uc_fill_pager_atom_data(str + 9, 8);
	}

	uc_check_paint(UC_PAGER_LINE_NR, 8);

	for (s = 0; s < (UC_PAGER_LINE_NR / 2); s += 2)
		free(uc_data.exp_ln[s].str);
}
END_TEST

START_TEST(uc_pager_nobreak_line_indent)
{
	unsigned int s;
	size_t       len;

	for (s = 0; s < UC_PAGER_LINE_NR; s++) {
		char*  str;

		len = asprintf(&str, "string %d", s);

		ck_assert_int_eq(pg_feed_line(&uc_pager,
		                              UC_PAGER_LINE_NR - (s + 1)), 0);
		uc_fill_pager_line_data(str,
		                        str,
		                        len,
		                        UC_PAGER_LINE_NR - (s + 1),
		                        s,
		                        s + 1);
		uc_fill_pager_atom_data(str, len);
	}

	uc_check_paint(UC_PAGER_LINE_NR, len + UC_PAGER_LINE_NR - 1);

	for (s = 0; s < UC_PAGER_LINE_NR; s++)
		free(uc_data.exp_ln[s].str);
}
END_TEST

START_TEST(uc_pager_nobreak_string_postindent)
{
	unsigned int s;
	size_t       len;

	for (s = 0; s < UC_PAGER_LINE_NR; s++) {
		char*  str;

		len = asprintf(&str, "\n  string %d", s);

		uc_fill_pager_line_data(str, str + 3, len - 3, 2, s, s + 1);
		uc_fill_pager_atom_data(str + 3, len - 3);
	}

	uc_check_paint(UC_PAGER_LINE_NR, len - 1);

	for (s = 0; s < UC_PAGER_LINE_NR; s++)
		free(uc_data.exp_ln[s].str - 3);
}
END_TEST

START_TEST(uc_pager_nobreak_string_preindent)
{
	unsigned int s;
	size_t       len;

	for (s = 0; s < UC_PAGER_LINE_NR; s++) {
		char*  str;

		len = asprintf(&str, "  string %d\n", s);

		uc_fill_pager_line_data(str, str + 2, len - 3, 2, s, s + 2);
		uc_fill_pager_atom_data(str + 2, len - 3);
	}

	uc_check_paint(UC_PAGER_LINE_NR + 1, len - 1);

	for (s = 0; s < UC_PAGER_LINE_NR; s++)
		free(uc_data.exp_ln[s].str - 2);
}
END_TEST

START_TEST(uc_pager_nobreak_mix_indent)
{
	unsigned int s;
	size_t       len;

	for (s = 0; s < UC_PAGER_LINE_NR; s++) {
		char*  str;

		len = asprintf(&str, "    string %d", s);

		ck_assert_int_eq(pg_feed_line(&uc_pager, 2), 0);
		uc_fill_pager_line_data(str,
		                        str + 4,
		                        len - 4,
		                        6,
		                        s,
		                        s + 1);
		uc_fill_pager_atom_data(str + 4, len - 4);
	}

	uc_check_paint(UC_PAGER_LINE_NR, len + 2);

	for (s = 0; s < UC_PAGER_LINE_NR; s++)
		free(uc_data.exp_ln[s].str - 4);
}
END_TEST

START_TEST(uc_pager_nobreak_trailing_blanks)
{
	char* const str = "sample string  ";
	size_t      len = strlen(str);

	uc_fill_pager_line_data(str, str, len - 2, 0, 0, 1);
	uc_fill_pager_atom_data(str, len - 2);

	ck_assert_int_eq(pg_feed_line(&uc_pager, 0), 0);

	uc_fill_pager_line_data(str, str, len - 2, 0, 1, 2);
	uc_fill_pager_atom_data(str, len - 2);

	uc_check_paint(2, len - 2);
}
END_TEST

START_TEST(uc_pager_nobreak_posttrailing_blanks)
{
	char* const str = "\n   ";

	uc_fill_pager_line_data(str, NULL, 0, 3, 0, 1);
	uc_fill_pager_line_data(str, NULL, 0, 3, 1, 2);

	uc_check_paint(2, 0);
}
END_TEST

static void uc_pager_nobreak_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Pager no automatic break");
	tcase_add_checked_fixture(tc,
	                          uc_pager_setup,
	                          uc_pager_teardown);

	tcase_add_test(tc, uc_pager_nobreak_multi_line);
	tcase_add_test(tc, uc_pager_nobreak_prenl);
	tcase_add_test(tc, uc_pager_nobreak_postnl);
	tcase_add_test(tc, uc_pager_nobreak_multi_atom_nl);
	tcase_add_test(tc, uc_pager_nobreak_line_indent);
	tcase_add_test(tc, uc_pager_nobreak_string_postindent);
	tcase_add_test(tc, uc_pager_nobreak_string_preindent);
	tcase_add_test(tc, uc_pager_nobreak_mix_indent);
	tcase_add_test(tc, uc_pager_nobreak_trailing_blanks);
	tcase_add_test(tc, uc_pager_nobreak_posttrailing_blanks);
	suite_add_tcase(suite, tc);
}

static Suite* uc_pager_suite(void)
{
	Suite* s;

	s = suite_create("Pager");

	uc_pager_life_cycle_tcase(s);
	uc_pager_nobreak_tcase(s);

	return s;
}

int main(int argc, char const* argv[])
{
	Suite*   s;
	SRunner* r;
	int      cnt;

	s = uc_pager_suite();
	r = srunner_create(s);

	if (argc == 2) {
		srunner_set_fork_status(r, CK_NOFORK);
		srunner_run(r, NULL, argv[1], CK_VERBOSE);
	}
	else if (argc == 1)
		srunner_run_all(r, CK_VERBOSE);
	else
		fprintf(stderr, "invalid number of arguments\n");

	cnt = srunner_ntests_failed(r);
	srunner_free(r);

	return (!cnt) ? EXIT_SUCCESS : EXIT_FAILURE;
}
