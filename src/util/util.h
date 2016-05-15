#pragma once

#include <stddef.h>
#include <climits>

typedef size_t index_t;
typedef unsigned long cursor_t;
cursor_t const CURSOR_MAX = ULONG_MAX;

void debug(char const *format, ...);

template<typename T> class Bounds {

public:

    T const _min, _max;

    Bounds(T min, T max)
            : _min(min), _max(max) {
    }

    T clamp(T x) const {
        return _min <= x ? (x <= _max ? x : _max) : _min;
    }

};
