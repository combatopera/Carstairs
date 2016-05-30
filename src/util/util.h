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

#define error(...) fprintf(stderr, "ERROR %s ", LOG.name()); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#define warn(...) fprintf(stderr, "WARN %s ", LOG.name()); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#define info(...) fprintf(stderr, "INFO %s ", LOG.name()); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#ifndef INFO
#define debug(...) fprintf(stderr, "DEBUG %s ", LOG.name()); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#else
#define debug(...)
#endif
#ifdef TRACE
#define trace(...) fprintf(stderr, "TRACE %s ", LOG.name()); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")
#else
#define trace(...)
#endif

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
