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

    char const *_name;

    Log(char const * const file)
            : _name(createName(file)) {
    }

    char const *name() const {
        return _name;
    }

    ~Log() {
        delete[] _name;
        _name = 0; // Avoid garbage in subsequent logging.
    }

};

#define CARSTAIRS_ERROR(format, ...) fprintf(stderr, "ERROR %s " format "\n", LOG.name(), ##__VA_ARGS__)
#define CARSTAIRS_WARN(format, ...) fprintf(stderr, "WARN %s " format "\n", LOG.name(), ##__VA_ARGS__)
#define CARSTAIRS_INFO(format, ...) fprintf(stderr, "INFO %s " format "\n", LOG.name(), ##__VA_ARGS__)
#ifndef CARSTAIRS_LEVEL_INFO
#define CARSTAIRS_DEBUG(format, ...) fprintf(stderr, "DEBUG %s " format "\n", LOG.name(), ##__VA_ARGS__)
#else
#define CARSTAIRS_DEBUG(format, ...)
#endif
#ifdef CARSTAIRS_LEVEL_TRACE
#define CARSTAIRS_TRACE(format, ...) fprintf(stderr, "TRACE %s " format "\n", LOG.name(), ##__VA_ARGS__)
#else
#define CARSTAIRS_TRACE(format, ...)
#endif

#define CARSTAIRS_REFRESH(T, ...) this->~T(); \
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

#ifdef CARSTAIRS_TEST
#define CARSTAIRS_CONST
#else
#define CARSTAIRS_CONST const
#endif
