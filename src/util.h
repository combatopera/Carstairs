#pragma once

#include <stddef.h>

template<typename T> class View {

    bool _master;

protected:

    size_t _limit, _capacity;

    T *_data;

public:

    View();

    View(const View<T>& master);

    ~View();

    size_t limit();

    void copy(T *to);

};

template<typename T> class Buffer: public View<T> {

public:

    void setLimit(size_t limit);

    void zero();

};

#define UTIL_INSTANTIATE(T) \
    template class Buf<T>; \
    template class MasterBuf<T>;
