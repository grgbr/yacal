/**
 * @file
 * @brief List scroller interface
 */
#ifndef _SCROLLER_H
#define _SCROLLER_H

#include "utils.h"

/**
 * @brief List scroller painting operation
 *
 * @details List scroller painting / display operation descriptor
 */
struct scroller_operation {
	/** @brief Number of (visible) rows affected. */
	unsigned int scrl_rows_cnt;
	/** @brief First (visible) data entry affected. */
	unsigned int scrl_first_ent;
	/**
	 * @brief First (visible) row affected.
	 * @details Corresponds to first data entry indexed by scrl_first_ent.
	 */
	unsigned int scrl_first_row;
	/** @brief Row to be selected / highlighted. */
	unsigned int scrl_sel_row;
};

/**
 * @brief List scroller
 *
 * @details Hold state informations needed to handle scrolling list.
 */
struct scroller {
	/** @brief Data entry displayed at the top (visible) row. */
	unsigned int scrl_top_ent;
	/** @brief Numbers of rows available for display. */
	unsigned int scrl_rows_nr;
	/** @brief Numbers rows available for display. */
	unsigned int scrl_cols_nr;
};

/**
 * @brief Get current number of columns for list scroller passed in argument.
 *
 * @param[in] thiz List scroller
 * @return number of columns
 */
static inline __nonull(1) __nothrow __pure
unsigned int scrl_columns_nr(struct scroller const* thiz)
{
	return thiz->scrl_cols_nr;
}

/**
 * @brief Get current number of rows for list scroller passed in argument.
 *
 * @param[in] thiz List scroller
 * @return number of rows
 */
static inline __nonull(1) __nothrow __pure
unsigned int scrl_rows_nr(struct scroller const* thiz)
{
	return thiz->scrl_rows_nr;
}

/**
 * @brief Reset list scroller
 *
 * @param[in,out] thiz         List scroller
 * @param[in]     select_entry Initial selected / highlighted entry index
 *
 * @details Reset scroller to a known default state, initially highlighting
 * entry passed in argument.
 */
static inline __nonull(1) __nothrow
void scrl_reset(struct scroller* thiz, unsigned int select_entry)
{
	thiz->scrl_top_ent = select_entry;
	thiz->scrl_rows_nr = 0;
	thiz->scrl_cols_nr = 0;
}

/**
 * @brief Render list scroller
 *
 * @param[in,out] thiz          List scroller
 * @param[in]     select_entry  Index of item to select / highlight
 * @param[in]     entries_count Total number of list item
 * @param[in]     columns_nr    Number of columns available to paint items
 * @param[in]     rows_nr       Number of rows available to paint item
 * @param[out]    operation     Painting operation descriptor
 *
 * @details Fill in \p operation, describing how to repaint list scroller \p
 * thiz. \p operation is meant to be further passed as argument to scrl_paint().
 */
extern void scrl_render(struct scroller*           thiz,
                        unsigned int               select_entry,
                        unsigned int               entries_count,
                        unsigned int               columns_nr,
                        unsigned int               rows_nr,
                        struct scroller_operation* operation)
            __nonull(1, 6) __nothrow __leaf;

/** @brief List scroller entry paint implementor */
typedef int (scrl_paint_entry_fn)(unsigned int, unsigned int, void*) ;

/** @brief List scroller row selection implementor */
typedef int (scrl_select_row_fn)(unsigned int, void*);

/**
 * @brief Paint / display list scroller
 *
 * @param[in]     operation   List scroller operation
 * @param[in]     paint_entry Function painting an entry
 * @param[in]     select_row  Function selecting / highlighting a row
 * @param[in,out] data        Arbitrary data passed to \p paint_entry and
 *                            \p select_row
 * @return 0 if success, an errno like error code otherwise
 *
 * @details Use \p paint_entry and \p select_row passed in argument to paint /
 * display list scroller content described by \p operation argument.
 */
extern int scrl_paint(struct scroller_operation const* operation,
                      scrl_paint_entry_fn*             paint_entry,
                      scrl_select_row_fn*              select_row,
                      void*                            data)
           __nonull(1, 2, 3) __nothrow __leaf;

#endif
