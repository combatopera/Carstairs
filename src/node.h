#pragma once

#include "buf.h"
#include "state.h"
#include "util/util.h"

template<typename T> class Node {

public:

    Node(const char *label, State *state);

    virtual ~Node();

    void reset() {
        _cursor = 0;
        resetImpl();
    }

    virtual View<T> render(cursor_t newCursor);

    cursor_t cursor() {
        return _cursor;
    }

protected:

    Buffer<T> _buf;

    State *_state;

    cursor_t _cursor;

    virtual void resetImpl() = 0;

    virtual void renderImpl() = 0;

};

#define NODE_INSTANTIATE(T) \
    template Node<T>::Node(const char *, State *); \
    template Node<T>::~Node();
