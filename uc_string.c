#include <stdio.h>
#include <check.h>
#include "string.h"

/******************************************************************************
 * Count leading blank
 ******************************************************************************/

START_TEST(uc_str_count_leading_blank_ctrl)
{
	char const* const str = "\a";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_alnum)
{
	char const* const str = "alphanum";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_punct)
{
	char const* const str = ",";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_nl)
{
	char const* const str = "\n";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_formfeed)
{
	char const* const str = "\f";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_carriage)
{
	char const* const str = "\r";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_vtab)
{
	char const* const str = "\v";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_escape)
{
	char const* const str = "\033";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_space)
{
	char const* const str = " ";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_blank_tabulation)
{
	char const* const str = "\t";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_blank_false)
{
	char const* const str = "; sample string";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_blank_one)
{
	char const* const str = " sample string";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_blank_multiple)
{
	char const* const str = "    sample  string ";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 4);
}
END_TEST

START_TEST(uc_str_count_leading_blank_mix)
{
	char const* const str = "  \t  \t ";

	ck_assert_uint_eq(str_count_leading_blank(str, strlen(str)), 7);
}
END_TEST

static void uc_str_count_leading_blank_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Count leading blanks");

	tcase_add_test(tc, uc_str_count_leading_blank_ctrl);
	tcase_add_test(tc, uc_str_count_leading_blank_alnum);
	tcase_add_test(tc, uc_str_count_leading_blank_punct);
	tcase_add_test(tc, uc_str_count_leading_blank_nl);
	tcase_add_test(tc, uc_str_count_leading_blank_formfeed);
	tcase_add_test(tc, uc_str_count_leading_blank_carriage);
	tcase_add_test(tc, uc_str_count_leading_blank_vtab);
	tcase_add_test(tc, uc_str_count_leading_blank_escape);
	tcase_add_test(tc, uc_str_count_leading_blank_space);
	tcase_add_test(tc, uc_str_count_leading_blank_tabulation);
	tcase_add_test(tc, uc_str_count_leading_blank_false);
	tcase_add_test(tc, uc_str_count_leading_blank_one);
	tcase_add_test(tc, uc_str_count_leading_blank_multiple);
	tcase_add_test(tc, uc_str_count_leading_blank_mix);
	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Count trailing blank
 ******************************************************************************/

START_TEST(uc_str_count_trailing_blank_ctrl)
{
	char const* const str = "\a";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_alnum)
{
	char const* const str = "alphanum";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_punct)
{
	char const* const str = ",";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_nl)
{
	char const* const str = "\n";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_formfeed)
{
	char const* const str = "\f";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_carriage)
{
	char const* const str = "\r";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_vtab)
{
	char const* const str = "\v";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_escape)
{
	char const* const str = "\033";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_space)
{
	char const* const str = " ";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_tabulation)
{
	char const* const str = "\t";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_false)
{
	char const* const str = "sample string ;";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_one)
{
	char const* const str = "sample string ";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_multiple)
{
	char const* const str = " sample  string    ";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 4);
}
END_TEST

START_TEST(uc_str_count_trailing_blank_mix)
{
	char const* const str = "  \t  \t ";

	ck_assert_uint_eq(str_count_trailing_blank(str, strlen(str)), 7);
}
END_TEST

static void uc_str_count_trailing_blank_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Count trailing blanks");

	tcase_add_test(tc, uc_str_count_trailing_blank_ctrl);
	tcase_add_test(tc, uc_str_count_trailing_blank_alnum);
	tcase_add_test(tc, uc_str_count_trailing_blank_punct);
	tcase_add_test(tc, uc_str_count_trailing_blank_nl);
	tcase_add_test(tc, uc_str_count_trailing_blank_formfeed);
	tcase_add_test(tc, uc_str_count_trailing_blank_carriage);
	tcase_add_test(tc, uc_str_count_trailing_blank_vtab);
	tcase_add_test(tc, uc_str_count_trailing_blank_escape);
	tcase_add_test(tc, uc_str_count_trailing_blank_space);
	tcase_add_test(tc, uc_str_count_trailing_blank_tabulation);
	tcase_add_test(tc, uc_str_count_trailing_blank_false);
	tcase_add_test(tc, uc_str_count_trailing_blank_one);
	tcase_add_test(tc, uc_str_count_trailing_blank_multiple);
	tcase_add_test(tc, uc_str_count_trailing_blank_mix);
	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Count leading non blank
 ******************************************************************************/

START_TEST(uc_str_count_leading_noblank_ctrl)
{
	char const* const str = "\a";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_alnum)
{
	char const* const str = "alphanum";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_punct)
{
	char const* const str = ",";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_nl)
{
	char const* const str = "\n";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_formfeed)
{
	char const* const str = "\f";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_carriage)
{
	char const* const str = "\r";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_vtab)
{
	char const* const str = "\v";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_escape)
{
	char const* const str = "\033";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noblank_space)
{
	char const* const str = " ";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_noblank_tabulation)
{
	char const* const str = "\t";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_noblank_false)
{
	char const* const str = "\tsample string";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_noblank_one)
{
	char const* const str = "; sample string";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noblank_multiple)
{
	char const* const str = "sample  string ";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)), 6);
}
END_TEST

START_TEST(uc_str_count_leading_noblank_mix)
{
	char const* const str = "  \tsample string\t ";

	ck_assert_uint_eq(str_count_leading_noblank(str, strlen(str)), 0);
}
END_TEST

static void uc_str_count_leading_noblank_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Count leading non blank");

	tcase_add_test(tc, uc_str_count_leading_noblank_ctrl);
	tcase_add_test(tc, uc_str_count_leading_noblank_alnum);
	tcase_add_test(tc, uc_str_count_leading_noblank_punct);
	tcase_add_test(tc, uc_str_count_leading_noblank_nl);
	tcase_add_test(tc, uc_str_count_leading_noblank_formfeed);
	tcase_add_test(tc, uc_str_count_leading_noblank_carriage);
	tcase_add_test(tc, uc_str_count_leading_noblank_vtab);
	tcase_add_test(tc, uc_str_count_leading_noblank_escape);
	tcase_add_test(tc, uc_str_count_leading_noblank_space);
	tcase_add_test(tc, uc_str_count_leading_noblank_tabulation);
	tcase_add_test(tc, uc_str_count_leading_noblank_false);
	tcase_add_test(tc, uc_str_count_leading_noblank_one);
	tcase_add_test(tc, uc_str_count_leading_noblank_multiple);
	tcase_add_test(tc, uc_str_count_leading_noblank_mix);
	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Count leading alphanumeric
 ******************************************************************************/

START_TEST(uc_str_count_leading_alnum_ctrl)
{
	char const* const str = "\a";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_alnum)
{
	char const* const str = "alphanum";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_alnum_punct)
{
	char const* const str = ",";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_nl)
{
	char const* const str = "\n";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_formfeed)
{
	char const* const str = "\f";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_carriage)
{
	char const* const str = "\r";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_vtab)
{
	char const* const str = "\v";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_escape)
{
	char const* const str = "\033";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_space)
{
	char const* const str = " ";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_tabulation)
{
	char const* const str = "\t";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_false)
{
	char const* const str = "; sample string";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_one)
{
	char const* const str = "s ample string";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_alnum_multiple)
{
	char const* const str = "sample  string ";

	ck_assert_uint_eq(str_count_leading_alnum(str, strlen(str)), 6);
}
END_TEST

static void uc_str_count_leading_alnum_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Count leading alphanumeric");

	tcase_add_test(tc, uc_str_count_leading_alnum_ctrl);
	tcase_add_test(tc, uc_str_count_leading_alnum_alnum);
	tcase_add_test(tc, uc_str_count_leading_alnum_punct);
	tcase_add_test(tc, uc_str_count_leading_alnum_nl);
	tcase_add_test(tc, uc_str_count_leading_alnum_formfeed);
	tcase_add_test(tc, uc_str_count_leading_alnum_carriage);
	tcase_add_test(tc, uc_str_count_leading_alnum_vtab);
	tcase_add_test(tc, uc_str_count_leading_alnum_escape);
	tcase_add_test(tc, uc_str_count_leading_alnum_space);
	tcase_add_test(tc, uc_str_count_leading_alnum_tabulation);
	tcase_add_test(tc, uc_str_count_leading_alnum_false);
	tcase_add_test(tc, uc_str_count_leading_alnum_one);
	tcase_add_test(tc, uc_str_count_leading_alnum_multiple);
	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Count leading non alphanumeric
 ******************************************************************************/

START_TEST(uc_str_count_leading_noalnum_ctrl)
{
	char const* const str = "\a";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)),
	                  strlen(str));
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_alnum)
{
	char const* const str = "alphanum";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_punct)
{
	char const* const str = ",";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_nl)
{
	char const* const str = "\n";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_formfeed)
{
	char const* const str = "\f";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_carriage)
{
	char const* const str = "\r";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_vtab)
{
	char const* const str = "\v";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_escape)
{
	char const* const str = "\033";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_space)
{
	char const* const str = " ";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_tabulation)
{
	char const* const str = "\t";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_false)
{
	char const* const str = "s sample string";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_leading_noalnum_two)
{
	char const* const str = "; sample string";

	ck_assert_uint_eq(str_count_leading_noalnum(str, strlen(str)), 2);
}
END_TEST

static void uc_str_count_leading_noalnum_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Count leading non alphanumeric");

	tcase_add_test(tc, uc_str_count_leading_noalnum_ctrl);
	tcase_add_test(tc, uc_str_count_leading_noalnum_alnum);
	tcase_add_test(tc, uc_str_count_leading_noalnum_punct);
	tcase_add_test(tc, uc_str_count_leading_noalnum_nl);
	tcase_add_test(tc, uc_str_count_leading_noalnum_formfeed);
	tcase_add_test(tc, uc_str_count_leading_noalnum_carriage);
	tcase_add_test(tc, uc_str_count_leading_noalnum_vtab);
	tcase_add_test(tc, uc_str_count_leading_noalnum_escape);
	tcase_add_test(tc, uc_str_count_leading_noalnum_space);
	tcase_add_test(tc, uc_str_count_leading_noalnum_tabulation);
	tcase_add_test(tc, uc_str_count_leading_noalnum_false);
	tcase_add_test(tc, uc_str_count_leading_noalnum_two);
	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Count trailing alphanumeric
 ******************************************************************************/

START_TEST(uc_str_count_trailing_alnum_ctrl)
{
	char const* const str = "\a";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_alnum)
{
	char const* const str = "alphanum";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 8);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_punct)
{
	char const* const str = ",";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_nl)
{
	char const* const str = "\n";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_formfeed)
{
	char const* const str = "\f";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_carriage)
{
	char const* const str = "\r";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_vtab)
{
	char const* const str = "\v";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_escape)
{
	char const* const str = "\033";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_space)
{
	char const* const str = " ";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_tabulation)
{
	char const* const str = "\t";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_false)
{
	char const* const str = "sample string ;";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_one)
{
	char const* const str = "sample s";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_trailing_alnum_multiple)
{
	char const* const str = " sample  string";

	ck_assert_uint_eq(str_count_trailing_alnum(str, strlen(str)), 6);
}
END_TEST

static void uc_str_count_trailing_alnum_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Count trailing alphanumeric");

	tcase_add_test(tc, uc_str_count_trailing_alnum_ctrl);
	tcase_add_test(tc, uc_str_count_trailing_alnum_alnum);
	tcase_add_test(tc, uc_str_count_trailing_alnum_punct);
	tcase_add_test(tc, uc_str_count_trailing_alnum_nl);
	tcase_add_test(tc, uc_str_count_trailing_alnum_formfeed);
	tcase_add_test(tc, uc_str_count_trailing_alnum_carriage);
	tcase_add_test(tc, uc_str_count_trailing_alnum_vtab);
	tcase_add_test(tc, uc_str_count_trailing_alnum_escape);
	tcase_add_test(tc, uc_str_count_trailing_alnum_space);
	tcase_add_test(tc, uc_str_count_trailing_alnum_tabulation);
	tcase_add_test(tc, uc_str_count_trailing_alnum_false);
	tcase_add_test(tc, uc_str_count_trailing_alnum_one);
	tcase_add_test(tc, uc_str_count_trailing_alnum_multiple);
	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Count blank duplicates
 ******************************************************************************/

START_TEST(uc_str_count_blank_dups_none)
{
	char const* const str   = "nonblank";

	ck_assert_uint_eq(str_count_blank_dups(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_blank_dups_single_lead)
{
	char const* const str   = " nonblank";

	ck_assert_uint_eq(str_count_blank_dups(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_blank_dups_single_trail)
{
	char const* const str   = "nonblank ";

	ck_assert_uint_eq(str_count_blank_dups(str, strlen(str)), 0);
}
END_TEST

START_TEST(uc_str_count_blank_dups_multiple_lead)
{
	char const* const str   = "  nonblank";

	ck_assert_uint_eq(str_count_blank_dups(str, strlen(str)), 1);
}
END_TEST

START_TEST(uc_str_count_blank_dups_multiple_trail)
{
	char const* const str   = "nonblank    ";

	ck_assert_uint_eq(str_count_blank_dups(str, strlen(str)), 3);
}
END_TEST

START_TEST(uc_str_count_blank_dups_only)
{
	char const* const str   = "   ";

	ck_assert_uint_eq(str_count_blank_dups(str, strlen(str)), 2);
}
END_TEST


static void uc_str_count_blank_dups(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Count blank duplicates");

	tcase_add_test(tc, uc_str_count_blank_dups_none);
	tcase_add_test(tc, uc_str_count_blank_dups_single_lead);
	tcase_add_test(tc, uc_str_count_blank_dups_single_trail);
	tcase_add_test(tc, uc_str_count_blank_dups_multiple_lead);
	tcase_add_test(tc, uc_str_count_blank_dups_multiple_trail);
	tcase_add_test(tc, uc_str_count_blank_dups_only);

	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Squeeze blank characters
 ******************************************************************************/

START_TEST(uc_str_squeeze_blanks_noblank)
{
	char const* const src   = "sample";
	char              dst[] = "______";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)), 6);
	ck_assert_str_eq(src, dst);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_blankonly)
{
	char const* const src   = "      ";
	char              dst[] = "______";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)), 1);
	ck_assert_int_eq(strncmp(src, dst, 1), 0);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_single)
{
	char const* const src   = "sample string";
	char              dst[] = "_____________";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(src));
	ck_assert_str_eq(src, dst);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_before_after)
{
	char const* const src   = " sample string ";
	char              dst[] = "_______________";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(src));
	ck_assert_str_eq(src, dst);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_before)
{
	char const* const src   = "    sample string";
	char              dst[] = "_________________";
	char const* const exp   = " sample string";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(exp));
	ck_assert_int_eq(strncmp(dst, exp, strlen(exp)), 0);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_after)
{
	char const* const src   = "sample string       ";
	char              dst[] = "____________________";
	char const* const exp   = "sample string ";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(exp));
	ck_assert_int_eq(strncmp(dst, exp, strlen(exp)), 0);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_triple)
{
	char const* const src   = " sample   string ";
	char              dst[] = "_________________";
	char const* const exp   = " sample string ";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(exp));
	ck_assert_int_eq(strncmp(dst, exp, strlen(exp)), 0);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_before_triple_after)
{
	char const* const src   = "   sample   string   ";
	char              dst[] = "_____________________";
	char const* const exp   = " sample string ";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(exp));
	ck_assert_int_eq(strncmp(dst, exp, strlen(exp)), 0);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_quad)
{
	char const* const src   = "   sample   string sample2  string2   ";
	char              dst[] = "______________________________________";
	char const* const exp   = " sample string sample2 string2 ";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(exp));
	ck_assert_int_eq(strncmp(dst, exp, strlen(exp)), 0);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_tab)
{
	char const* const src   = "   sample \t string sample2  string2   ";
	char              dst[] = "_______________________________________";
	char const* const exp   = " sample string sample2 string2 ";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(exp));
	ck_assert_int_eq(strncmp(dst, exp, strlen(exp)), 0);
}
END_TEST

START_TEST(uc_str_squeeze_blanks_punct)
{
	char const* const src   = "   sample,  string sample2  ; string2  ;";
	char              dst[] = "_______________________________________";
	char const* const exp   = " sample, string sample2 ; string2 ;";

	ck_assert_uint_eq(str_squeeze_blanks(dst, src, strlen(src)),
	                  strlen(exp));
	ck_assert_int_eq(strncmp(dst, exp, strlen(exp)), 0);
}
END_TEST

static void uc_str_squeeze_blanks_tcase(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Squeeze blanks");

	tcase_add_test(tc, uc_str_squeeze_blanks_noblank);
	tcase_add_test(tc, uc_str_squeeze_blanks_blankonly);
	tcase_add_test(tc, uc_str_squeeze_blanks_single);
	tcase_add_test(tc, uc_str_squeeze_blanks_before_after);
	tcase_add_test(tc, uc_str_squeeze_blanks_before);
	tcase_add_test(tc, uc_str_squeeze_blanks_after);
	tcase_add_test(tc, uc_str_squeeze_blanks_triple);
	tcase_add_test(tc, uc_str_squeeze_blanks_before_triple_after);
	tcase_add_test(tc, uc_str_squeeze_blanks_quad);
	tcase_add_test(tc, uc_str_squeeze_blanks_tab);
	tcase_add_test(tc, uc_str_squeeze_blanks_punct);

	suite_add_tcase(suite, tc);
}

/******************************************************************************
 * Evaluate squeezed blanks maximum length
 ******************************************************************************/

START_TEST(uc_str_eval_squeeze_blanks_only)
{
	char const* const str   = "     ";
	size_t            avail = 1;

	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 5);
	ck_assert_uint_eq(avail, 0);

	avail = 4;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 5);
	ck_assert_uint_eq(avail, 3);
}
END_TEST

START_TEST(uc_str_eval_squeeze_blanks_none)
{
	char const* const str   = "sample";
	size_t            avail = 1;

	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 1);
	ck_assert_uint_eq(avail, 0);

	avail = 4;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 4);
	ck_assert_uint_eq(avail, 0);

	avail = 5;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 5);
	ck_assert_uint_eq(avail, 0);

	avail = 7;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 6);
	ck_assert_uint_eq(avail, 1);
}
END_TEST

START_TEST(uc_str_eval_squeeze_blanks_lead)
{
	char const* const str   = "  sample";
	size_t            avail = 1;

	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 2);
	ck_assert_uint_eq(avail, 0);

	avail = 6;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 7);
	ck_assert_uint_eq(avail, 0);

	avail = 10;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 8);
	ck_assert_uint_eq(avail, 3);
}
END_TEST

START_TEST(uc_str_eval_squeeze_blanks_trail)
{
	char const* const str   = "sample   ";
	size_t            avail = 1;

	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 1);
	ck_assert_uint_eq(avail, 0);

	avail = 7;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 9);
	ck_assert_uint_eq(avail, 0);

	avail = 8;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 9);
	ck_assert_uint_eq(avail, 1);
}
END_TEST

START_TEST(uc_str_eval_squeeze_blanks_single)
{
	char const* const str   = "sample string";
	size_t            avail = 1;

	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 1);
	ck_assert_uint_eq(avail, 0);

	avail = 7;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 7);
	ck_assert_uint_eq(avail, 0);

	avail = 14;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 13);
	ck_assert_uint_eq(avail, 1);
}
END_TEST

START_TEST(uc_str_eval_squeeze_blanks_multiple)
{
	char const* const str   = "sample   string";
	size_t            avail = 1;

	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 1);
	ck_assert_uint_eq(avail, 0);

	avail = 8;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 10);
	ck_assert_uint_eq(avail, 0);

	avail = 13;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 15);
	ck_assert_uint_eq(avail, 0);
}
END_TEST

START_TEST(uc_str_eval_squeeze_blanks_all)
{
	char const* const str   = " sample   string  ";
	size_t            avail = 1;

	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 1);
	ck_assert_uint_eq(avail, 0);

	avail = 8;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 10);
	ck_assert_uint_eq(avail, 0);

	avail = 14;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 16);
	ck_assert_uint_eq(avail, 0);

	avail = 15;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 18);
	ck_assert_uint_eq(avail, 0);

	avail = 20;
	ck_assert_uint_eq(str_eval_squeeze_blanks(str, strlen(str), &avail), 18);
	ck_assert_uint_eq(avail, 5);
}
END_TEST

static void uc_str_eval_squeeze_blanks(Suite* suite)
{
	TCase* tc;

	tc = tcase_create("Evaluate squeezed blanks maximum length");

	tcase_add_test(tc, uc_str_eval_squeeze_blanks_only);
	tcase_add_test(tc, uc_str_eval_squeeze_blanks_none);
	tcase_add_test(tc, uc_str_eval_squeeze_blanks_lead);
	tcase_add_test(tc, uc_str_eval_squeeze_blanks_trail);
	tcase_add_test(tc, uc_str_eval_squeeze_blanks_single);
	tcase_add_test(tc, uc_str_eval_squeeze_blanks_multiple);
	tcase_add_test(tc, uc_str_eval_squeeze_blanks_all);

	suite_add_tcase(suite, tc);
}

static Suite* uc_str_utils_suite(void)
{
	Suite* s;
	
	s = suite_create("String utils");

	uc_str_count_leading_blank_tcase(s);
	uc_str_count_trailing_blank_tcase(s);
	uc_str_count_leading_noblank_tcase(s);
	uc_str_count_leading_alnum_tcase(s);
	uc_str_count_leading_noalnum_tcase(s);
	uc_str_count_trailing_alnum_tcase(s);
	uc_str_count_blank_dups(s);
	uc_str_eval_squeeze_blanks(s);
	uc_str_squeeze_blanks_tcase(s);

	return s;
}

int main(int argc, char const* argv[])
{
	Suite*   s;
	SRunner* r;
	int      cnt;
	
	s = uc_str_utils_suite();
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
