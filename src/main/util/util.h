// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

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

    bool accept(T x) const {
        return _min <= x && x <= _max;
    }

};

#ifdef CARSTAIRS_TEST
#define CARSTAIRS_CONST
#else
#define CARSTAIRS_CONST const
#endif
