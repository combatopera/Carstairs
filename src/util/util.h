#pragma once

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

#ifdef DIZZYM_UNIT_TEST
#define final
#else
#define final const
#endif
