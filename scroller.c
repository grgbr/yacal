/**
 * @file
 * @brief List scroller implementation
 */
#include "scroller.h"

__nonull(1, 6) __nothrow __leaf
void scrl_render(struct scroller*           thiz,
                 unsigned int               select_entry,
                 unsigned int               entries_count,
                 unsigned int               columns_nr,
                 unsigned int               rows_nr,
                 struct scroller_operation* operation)
{
	ut_assert(thiz);
	ut_assert(entries_count);
	ut_assert(select_entry < entries_count);
	ut_assert(columns_nr);
	ut_assert(rows_nr);
	ut_assert(operation);

	if (select_entry < thiz->scrl_top_ent) {
		operation->scrl_first_ent = select_entry;
		thiz->scrl_top_ent = select_entry;
		goto full;
	}

	if (select_entry >= (thiz->scrl_top_ent + rows_nr)) {
		operation->scrl_first_ent = select_entry + 1 - rows_nr;
		thiz->scrl_top_ent = operation->scrl_first_ent;
		goto full;
	}

	if (columns_nr != thiz->scrl_cols_nr) {
		operation->scrl_first_ent = thiz->scrl_top_ent;
		goto full;
	}

	if (rows_nr <= thiz->scrl_rows_nr) {
		operation->scrl_rows_cnt = 0;
		thiz->scrl_rows_nr = ut_min(thiz->scrl_rows_nr, rows_nr);
		goto out;
	}

	operation->scrl_rows_cnt = ut_min(entries_count - thiz->scrl_top_ent,
	                                  rows_nr) -
	                           thiz->scrl_rows_nr;
	operation->scrl_first_ent = thiz->scrl_top_ent + thiz->scrl_rows_nr;
	operation->scrl_first_row = thiz->scrl_rows_nr;
	thiz->scrl_rows_nr += operation->scrl_rows_cnt;
	goto out;

full:
	operation->scrl_rows_cnt = ut_min(entries_count -
	                                  operation->scrl_first_ent,
	                                  rows_nr);
	operation->scrl_first_row = 0;
	thiz->scrl_rows_nr = operation->scrl_rows_cnt;

out:
	thiz->scrl_cols_nr = columns_nr;

	operation->scrl_sel_row = select_entry - thiz->scrl_top_ent;
}

__nonull(1, 2, 3) __nothrow __leaf
int scrl_paint(struct scroller_operation const* operation,
               scrl_paint_entry_fn*             paint_entry,
               scrl_select_row_fn*              select_row,
               void*                            data)
{
	ut_assert(operation);
	ut_assert(paint_entry);
	ut_assert(select_row);

	int          err;
	unsigned int c;
	unsigned int cnt = operation->scrl_rows_cnt;

	for (err = 0, c = 0; !err && (c < cnt); c++)
		err = paint_entry(operation->scrl_first_ent + c,
		                  operation->scrl_first_row + c,
		                  data);

	if (err)
		return err;

	return select_row(operation->scrl_sel_row, data);
}
