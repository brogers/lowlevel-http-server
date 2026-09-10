#ifndef UTIL_H
#define UTIL_H

#include <stdio.h> // IWYU pragma: keep

#define debug_log(s) fprintf(stderr, "[%s] %s\n", __FILE__, s)

char *loadfile(char *path);

#endif // !UTIL_H
