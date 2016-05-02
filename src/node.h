#pragma once

#include "buf.h"
#include "state.h"
#include "util/util.h"

template<typename T> class Node {

public:

    Node(const char *label, State *state);

    virtual ~Node();

    virtual View<T> render(unsigned long newCursor);

    cursor_t cursor() {
        return _cursor;
    }

protected:

    Buffer<T> _buf;

    State *_state;

    cursor_t _cursor;

    virtual void renderImpl() = 0;

};

#define NODE_INSTANTIATE(T) \
    template Node<T>::Node(const char *, State *); \
    template Node<T>::~Node();
