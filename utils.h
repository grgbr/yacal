#ifndef _UTILS_H
#define _UTILS_H

#include <assert.h>
#include <values.h>
#include <sys/types.h>

#define __nonull(...)    __attribute__((nonnull(__VA_ARGS__)))
#define __nothrow        __attribute__((nothrow))
#define __leaf           __attribute__((leaf))
#define __pure           __attribute__((pure))
#define __const          __attribute__((const))
#define __printf(...)    __attribute__((format(printf, __VA_ARGS__)))
#define __unused         __attribute__((unused))

#define ut_assert(_cond) assert(_cond)

#define __ut_stringify(__str) #__str
#define ut_stringify(_str)    __ut_stringify(_str)

#define ut_array_count(_arr) \
	(sizeof(_arr) / sizeof((_arr)[0]))

#define ut_max(_x, _y)                 \
	({                             \
		typeof(_x) __x = (_x); \
		typeof(_y) __y = (_y); \
		__x > __y ? __x : __y; \
	 })

#define ut_min(_x, _y)                 \
	({                             \
		typeof(_x) __x = (_x); \
		typeof(_y) __y = (_y); \
		__x < __y ? __x : __y; \
	 })

static inline __nothrow __const
unsigned int ut_lower_pow2(unsigned int value)
{
	ut_assert(value);

	return ((sizeof(value) * CHAR_BIT) - 1) - __builtin_clz(value);
}

static inline __nothrow __const
unsigned int ut_upper_pow2(unsigned int value)
{
	unsigned int const pow = ut_lower_pow2(value);

	return ut_lower_pow2(value + ((1U << pow) - 1));
}

extern int ut_join_path(char*,
                        size_t,
                        char const*,
                        size_t,
                        char const*,
                        size_t) __nonull(1, 3, 5) __nothrow __leaf;

#endif
