#pragma once

#include <stddef.h>

template<typename T> class Node {

    unsigned long _cursor;

    size_t _capacity;

public:

    Node();

    virtual ~Node();

    virtual T *render(unsigned long newCursor);

protected:

    T *_buffer;

    virtual void renderImpl(unsigned long n) = 0;

};

#define NODE_INSTANTIATE(T) \
    template Node<T>::Node(); \
    template Node<T>::~Node();
