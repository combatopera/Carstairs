#pragma once

#include "state.h"
#include "util/buf.h"
#include "util/util.h"

template<typename T> class Node {

    static cursor_t const INITIAL_CURSOR = 0;

    cursor_t _cursor;

public:

    Node(char const *label, State *state);

    virtual ~Node();

    void reset() {
        _cursor = INITIAL_CURSOR;
        resetImpl();
    }

    View<T> render(cursor_t newCursor);

    cursor_t cursor() {
        return _cursor;
    }

protected:

    Buffer<T> _buf;

    State *_state;

    virtual void resetImpl() = 0;

    virtual void renderImpl() = 0;

};

#define NODE_INSTANTIATE(T) \
    template Node<T>::Node(char const *, State *); \
    template Node<T>::~Node(); \
    template View<T> Node<T>::render(cursor_t);
