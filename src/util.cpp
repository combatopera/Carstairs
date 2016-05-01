#include "util.h"

#include <ladspa.h>
#include <stddef.h>
#include <cstring>

template<typename T>
void zero(T *buf, size_t n) {
    memset(buf, 0, n * sizeof *buf); // Not portable in float case.
}

template<typename T>
void copy(T *from, T *to, size_t n) {
    memcpy(to, from, n * sizeof *from);
}
template void copy<int>(int *,int *,size_t n);
template void copy<LADSPA_Data>(LADSPA_Data *,LADSPA_Data *,size_t n);

template void zero<int>(int *,size_t n);
template void zero<LADSPA_Data>(LADSPA_Data *,size_t n);
