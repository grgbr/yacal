#ifndef _VDIR_H
#define _VDIR_H

#include <libical/icalfileset.h>
#include "utils.h"

struct vdir {
	int          vdir_cnt;
	icalset**    vdir_set;
};

extern unsigned int  vdir_component_count(struct vdir const*,
                                          icalcomponent_kind) __nonull(1);
extern int           vdir_open(struct vdir*, char const*) __nonull(1, 2);
extern void          vdir_close(struct vdir const*) __nonull(1);

struct vdir_iterator {
	icalcomponent* vdir_cpnt;
	icalcompiter   vdir_cpntiter;
	icalsetiter    vdir_setiter;
	struct vdir*   vdir_dir;
	unsigned int   vdir_set;
	unsigned int   vdir_cnt;
};

extern icalcomponent* vdir_begin_iterator(struct vdir_iterator*,
                                          struct vdir*) __nonull(1, 2);
extern icalcomponent* vdir_next_component(struct vdir_iterator*) __nonull(1);

#endif
