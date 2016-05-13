#pragma once

#include <stddef.h>
#include <climits>

typedef size_t index_t;
typedef unsigned long cursor_t;
cursor_t const CURSOR_MAX = ULONG_MAX;

void debug(const char *format, ...);
