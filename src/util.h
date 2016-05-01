#pragma once

#include <stddef.h>

template<typename T> class Buf {

    bool _master;

protected:

    size_t _limit, _capacity;

    T *_data;

public:

    Buf();

    Buf(const Buf<T>& master);

    ~Buf();

    size_t limit();

    void copy(T *to);

};

template<typename T> class MasterBuf: public Buf<T> {

public:

    void setLimit(size_t limit);

    void zero();

};

#define UTIL_INSTANTIATE(T) \
    template class Buf<T>; \
    template class MasterBuf<T>;
