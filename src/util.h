#pragma once

#include <stddef.h>

template<typename T> class Buf {

    bool _master;

    size_t _limit, _capacity;

    T *_data;

public:

    Buf();

    Buf(const Buf<T>& master);

    ~Buf();

    void setLimit(size_t limit);

    size_t limit();

    void zero();

    void copy(T *to);

};

#define UTIL_INSTANTIATE(T) \
    template class Buf<T>;
