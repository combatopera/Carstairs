#include "util.h"

#include <ladspa.h>
#include <stddef.h>
#include <cstring>

template<typename T> void zero(T *buf, size_t n) {
    memset(buf, 0, n * sizeof *buf); // Not portable in float case.
}

template<typename T> void copy(T *from, T *to, size_t n) {
    memcpy(to, from, n * sizeof *from);
}

UTIL_INSTANTIATE(int)
UTIL_INSTANTIATE(LADSPA_Data)
