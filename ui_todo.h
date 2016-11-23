#ifndef _UI_TODO_H
#define _UI_TODO_H

#include "utils.h"

struct todo;
struct dstr;

extern char const* ui_todo_title(struct todo const*) __leaf;

extern char const* ui_todo_calendar(struct todo const*) __leaf;

#define UI_TODO_PRIO_LEN 3
extern void        ui_todo_priority(struct todo const*, char*, size_t)
                   __nonull(2) __leaf;

#define UI_TODO_PERCENT_LEN 6
extern void        ui_todo_percent(char*, unsigned int, unsigned int, size_t)
                   __nonull(1) __nothrow __leaf;

extern void        ui_todo_categories(struct todo*, struct dstr*) __nonull(2);

extern char const* ui_todo_location(struct todo const*) __leaf;

extern char const* ui_todo_description(struct todo const*) __leaf;

#endif
