#include <string.h>
#include <errno.h>
#include "utils.h"

__nonull(1, 3, 5) __nothrow __leaf
int ut_join_path(char       *path,
                 size_t      psize,
                 const char *dname,
                 size_t      dlen,
                 const char *fname,
                 size_t      flen)
{
	ut_assert(path);
	ut_assert(psize > 1);
	ut_assert(dname && (dname[0] != '\0'));
	ut_assert(dlen > 0);
	ut_assert(fname && (fname[0] != '\0'));
	ut_assert(flen > 0);

	size_t len;

	len = dlen + 1 + flen;
	if ((len + 1) > psize)
		return -ENAMETOOLONG;

	memcpy(path, dname, dlen);
	path[dlen] = '/';
	memcpy(&path[dlen + 1], fname, flen);
	path[len] = '\0';

	return 0;
}
