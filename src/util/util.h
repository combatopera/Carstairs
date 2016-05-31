#pragma once

#include <string>

class Log {

    static char *createName(std::string file) {
        auto const lastSlash = file.find_last_of("/\\");
        if (file.npos != lastSlash) {
            file.erase(0, lastSlash + 1);
        }
        auto const lastDot = file.find_last_of('.');
        if (file.npos != lastDot) {
            file.erase(lastDot);
        }
        auto const n = file.length();
        char * const name = new char[n + 1];
        file.copy(name, n);
        name[n] = 0;
        return name;
    }

public:

    char const * const _name;

    Log(char const * const file)
            : _name(createName(file)) {
    }

    char const *name() const {
        return _name;
    }

    ~Log() {
        delete[] _name;
    }

};

#define error(format, ...) fprintf(stderr, "ERROR %s " format "\n", LOG.name(), ##__VA_ARGS__)
#define warn(format, ...) fprintf(stderr, "WARN %s " format "\n", LOG.name(), ##__VA_ARGS__)
#define info(format, ...) fprintf(stderr, "INFO %s " format "\n", LOG.name(), ##__VA_ARGS__)
#ifndef INFO
#define debug(format, ...) fprintf(stderr, "DEBUG %s " format "\n", LOG.name(), ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif
#ifdef TRACE
#define trace(format, ...) fprintf(stderr, "TRACE %s " format "\n", LOG.name(), ##__VA_ARGS__)
#else
#define trace(format, ...)
#endif

#define REFRESH(T, ...) this->~T(); \
new (this) T(__VA_ARGS__); \
return *this

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
