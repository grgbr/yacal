#include <string.h>
#include "string.h"
#include "todo.h"
#include "ui_todo.h"

static char const ui_todo_prio_fmt[] = "%" ut_stringify(UI_TODO_PRIO_LEN) "d";

static char const ui_todo_percent_fmt[] = "(%u%%)";
static char const ui_todo_undef_percent[] = "(??%)";

static char const ui_todo_error_str[] = "???";

__leaf
char const* ui_todo_title(struct todo const* todo)
{
	char const* title = NULL;

	if (!todo)
		return ui_todo_error_str;

	title = todo_summary(todo);
	if (!title)
		return ui_todo_error_str;

	return title;
}

__leaf
char const* ui_todo_calendar(struct todo const* todo)
{
	char const* cal = NULL;

	if (!todo)
		return ui_todo_error_str;

	cal = todo_calendar(todo);
	if (!cal)
		return ui_todo_error_str;

	return cal;
}

__nonull(2) __leaf
void ui_todo_priority(struct todo const* todo, char* string, size_t size)
{
	ut_assert(string);
	ut_assert(size >= sizeof(ui_todo_error_str));

	int prio;

	if (!todo) {
		memcpy(string, ui_todo_error_str, sizeof(ui_todo_error_str));
		return;
	}

	prio = todo_priority(todo);
	if (prio >= 0 && prio < 1000) {
		snprintf(string, size, ui_todo_prio_fmt, prio);

		return;
	}

	string[0] = '\0';
}

__nonull(1) __nothrow __leaf
void ui_todo_percent(char*         string,
                     unsigned int  index,
                     unsigned int  count,
                     size_t        size)
{
	ut_assert(string);
	ut_assert(size >= sizeof(ui_todo_undef_percent));

	if (count) {
		snprintf(string,
		         size,
		         ui_todo_percent_fmt,
		         (index * 100) / count);

		return;
	}

	memcpy(string, ui_todo_undef_percent, sizeof(ui_todo_undef_percent));
}

static __nonull(1, 2) __nothrow
int ui_join_todo_cats(struct dstr* dest, char const* src)
{
	int err;

	err = dstr_cat(dest, ",");
	if (err)
		return err;

	err = dstr_cat(dest, src);
	if (err) {
		dstr_trunc(dest, dstr_len(dest) - 1);
		return err;
	}

	return 0;
}

__nonull(2)
void ui_todo_categories(struct todo* todo, struct dstr* categories)
{
	ut_assert(categories);

	char const* cat = NULL;

	if (todo)
		cat = todo_category_entry(todo, 0);

	if (cat) {
		int          err;
		unsigned int cnt;
		unsigned int c;

		err = dstr_cpy(categories, cat);
		if (err) {
			dstr_cpy(categories, ui_todo_error_str);
			return;
		}

		cnt = todo_category_count(todo);
		for (c = 1; c < cnt; c++) {
			cat = todo_category_entry(todo, c);
			if (!cat)
				err = ui_join_todo_cats(categories,
				                        ui_todo_error_str);
			else
				err = ui_join_todo_cats(categories, cat);

			if (err)
				break;
		}

		dstr_shrink(categories);

		return;
	}

	dstr_trunc(categories, 0);
}

__leaf
char const* ui_todo_location(struct todo const* todo)
{
	if (!todo)
		return ui_todo_error_str;

	/* Location is optional: return NULL if not found. */
	return todo_location(todo);
}

__leaf
char const* ui_todo_description(struct todo const* todo)
{
	if (!todo)
		return ui_todo_error_str;

	/* Description is optional: return NULL if not found. */
	return todo_description(todo);
}
