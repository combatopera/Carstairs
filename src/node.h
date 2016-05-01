#pragma once

#include <stddef.h>

#include "state.h"

template<typename T> class Node {

    unsigned long _cursor;

    size_t _capacity;

public:

    Node(State *state);

    virtual ~Node();

    virtual T *render(unsigned long newCursor);

protected:

    State *_state;

    T *_buffer;

    virtual void renderImpl(unsigned long n) = 0;

};

#define NODE_INSTANTIATE(T) \
    template Node<T>::Node(State *); \
    template Node<T>::~Node();
