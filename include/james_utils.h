#ifndef __JAMES_UTILS_H_JR
#define __JAMES_UTILS_H_JR

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define STDERR(message) \
    fprintf(stderr, "%s\n", message);

#define STDERR_F(fmt, ...) \
    fprintf(stderr, fmt "\n", __VA_ARGS__);

#define ERROR(fmt, ...) \
    fprintf(stderr, fmt "\n", __VA_ARGS__, strerror(errno));

#endif
