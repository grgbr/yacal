#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>
#include <sys/param.h>
#include "vdir.h"

static int ical2errno(void)
{
	switch (icalerrno) {
	case ICAL_BADARG_ERROR:
		return -EFAULT;
	case ICAL_NEWFAILED_ERROR:
	case ICAL_ALLOCATION_ERROR:
		return -ENOMEM;
	case ICAL_MALFORMEDDATA_ERROR:
		return -EBADMSG;
	case ICAL_PARSE_ERROR:
		return -ENOMSG;
	case ICAL_INTERNAL_ERROR:
		return -EBADFD;
	case ICAL_FILE_ERROR:
		return -errno;
	case ICAL_USAGE_ERROR:
		return -EINVAL;
	case ICAL_UNIMPLEMENTED_ERROR:
		return -ENOSYS;
	case ICAL_NO_ERROR:
		return 0;
	case ICAL_UNKNOWN_ERROR:
	default:
		return -EIO;
	}
}

/******************************************************************************
 * Vdir storage
 ******************************************************************************/

static __nonull(1)
int vdir_match_files(struct dirent const* dirent)
{
	ut_assert(dirent);

	if (dirent->d_type != DT_REG)
		return 0;

	return !fnmatch("*.ics",
	                dirent->d_name,
	                FNM_NOESCAPE | FNM_PATHNAME | FNM_PERIOD);
}

__nonull(1)
unsigned int vdir_component_count(struct vdir const* thiz,
                                  icalcomponent_kind kind)
{
	unsigned int cnt = 0;
	int          s;

	for (s = 0; s < thiz->vdir_cnt; s++)
		cnt += icalfileset_count_components(thiz->vdir_set[s], kind);

	return cnt;
}

__nonull(1, 2)
int vdir_open(struct vdir* thiz, char const* dirpath)
{
	ut_assert(thiz);
	ut_assert(dirpath && (dirpath[0] != '\0'));

	struct dirent** dirs;
	char*           path;
	int             ret;
	int             d;

	thiz->vdir_cnt = scandir(dirpath, &dirs, vdir_match_files, NULL);
	if (thiz->vdir_cnt < 0)
		return -errno;

	if (!thiz->vdir_cnt)
		/* TODO: implement add set method */
		return -ENODATA;

	thiz->vdir_set = malloc(thiz->vdir_cnt * sizeof(*thiz->vdir_set));
	if (!thiz->vdir_set) {
		ret = -ENOMEM;
		goto free_dirs;
	}

	path = malloc(MAXPATHLEN);
	if (!path) {
		free(thiz->vdir_set);
		ret = -ENOMEM;
		goto free_dirs;
	}

	for (d = 0; d < thiz->vdir_cnt; d++) {
		ret = ut_join_path(path, MAXPATHLEN,
		                   dirpath, strlen(dirpath),
		                   dirs[d]->d_name, strlen(dirs[d]->d_name));
		if (ret)
			break;

		thiz->vdir_set[d] = icalfileset_new_reader(path);
		if (!thiz->vdir_set[d]) {
			ret = ical2errno();
			break;
		}
	}

	if (d != thiz->vdir_cnt) {
		while (d--)
			icalfileset_free(thiz->vdir_set[d]);

		free(thiz->vdir_set);
		goto free_path;
	}

	ret = 0;

free_path:
	free(path);
free_dirs:
	for (d = 0; d < thiz->vdir_cnt; d++)
		free(dirs[d]);
	free(dirs);

	return ret;
}

__nonull(1)
void vdir_close(struct vdir const* thiz)
{
	ut_assert(thiz);
	ut_assert(thiz->vdir_cnt > 0);
	ut_assert(thiz->vdir_set);

	int d;

	for (d = 0; d < thiz->vdir_cnt; d++)
		icalfileset_free(thiz->vdir_set[d]);

	free(thiz->vdir_set);
}

/******************************************************************************
 * Vdir storage iterator
 ******************************************************************************/

static __nonull(1)
icalcomponent* vdir_first_subcpnt(struct vdir_iterator* thiz,
                                  icalcomponent*        component)
{
	ut_assert(thiz);

	if (!component)
		return NULL;

	thiz->vdir_cpntiter = icalcomponent_begin_component(component,
	                                                    ICAL_ANY_COMPONENT);

	return icalcompiter_deref(&thiz->vdir_cpntiter);
}

static __nonull(1, 2)
icalcomponent* vdir_walk_cpnt(struct vdir_iterator* thiz, struct vdir* vdir)
{
	ut_assert(thiz);
	ut_assert(vdir);

	while (++thiz->vdir_set < (unsigned int)vdir->vdir_cnt) {
		thiz->vdir_setiter =
			icalfileset_begin_component(vdir->vdir_set[thiz->vdir_set],
			                            ICAL_VCALENDAR_COMPONENT,
			                            NULL);
		thiz->vdir_cpnt =
			vdir_first_subcpnt(thiz,
			                   icalsetiter_deref(&thiz->vdir_setiter));
		if (thiz->vdir_cpnt)
			return thiz->vdir_cpnt;
	}

	return NULL;
}

__nonull(1)
icalcomponent* vdir_next_component(struct vdir_iterator* thiz)
{
	ut_assert(thiz);

	if (thiz->vdir_cpnt) {
		thiz->vdir_cpnt = icalcompiter_next(&thiz->vdir_cpntiter);
		if (thiz->vdir_cpnt)
			return thiz->vdir_cpnt;
	}

	thiz->vdir_cpnt =
		vdir_first_subcpnt(thiz,
		                        icalsetiter_next(&thiz->vdir_setiter));
	if (thiz->vdir_cpnt)
		return thiz->vdir_cpnt;

	return vdir_walk_cpnt(thiz, thiz->vdir_dir);
}

__nonull(1, 2)
icalcomponent* vdir_begin_iterator(struct vdir_iterator* thiz,
                                   struct vdir*          vdir)
{
	ut_assert(thiz);
	ut_assert(vdir);

	thiz->vdir_setiter =
		icalfileset_begin_component(vdir->vdir_set[0],
	                                    ICAL_VCALENDAR_COMPONENT,
	                                    NULL);
	thiz->vdir_dir = vdir;
	thiz->vdir_set = 0;

	thiz->vdir_cpnt =
		vdir_first_subcpnt(thiz,
	                                icalsetiter_deref(&thiz->vdir_setiter));
	if (thiz->vdir_cpnt)
		return thiz->vdir_cpnt;

	return vdir_walk_cpnt(thiz, thiz->vdir_dir);
}
