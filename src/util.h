#pragma once

#include <stddef.h>

template<typename T> class View {

protected:

    View();

    size_t _limit;

    T *_data;

public:

    View(const View<T>& master);

    size_t limit();

    void copyTo(T *to);

};

template<typename T> class Buffer: public View<T> {

    size_t _capacity;

public:

    Buffer();

    ~Buffer();

    void setLimit(size_t limit);

    void zero();

};

#define UTIL_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
