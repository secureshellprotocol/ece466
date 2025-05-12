#ifndef __JAMES_UTILS_H_JR
#define __JAMES_UTILS_H_JR

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define STDERR(message) \
    fprintf(stderr, "%s: %s\n", __func__, message);

#define STDERR_F(fmt, ...) \
    fprintf(stderr, "%s: " fmt "\n", __func__ , __VA_ARGS__);

#define ERROR(fmt) \
    fprintf(stderr, "%s: %s \n", __func__, strerror(errno));

#endif
