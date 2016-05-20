#pragma once

#include <stddef.h>
#include <climits>

typedef size_t index_t;
size_t const SIZE_WRAP = size_t(0) - 1;

namespace DSSI {

typedef unsigned long cursor;
cursor const DSSI::CURSOR_MAX = ULONG_MAX;

}

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
