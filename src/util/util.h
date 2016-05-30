#pragma once

#define debug(...) fprintf(stderr, "[dizzYM] "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n")

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

#ifdef DIZZYM_UNIT_TEST
#define FINAL
#else
#define FINAL const
#endif
