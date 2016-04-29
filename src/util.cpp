#include <stddef.h>
#include <cstring>

template<typename T>
void zero(T *buf, size_t n) {
    memset(buf, 0, n * sizeof *buf); // Not portable in float case.
}
