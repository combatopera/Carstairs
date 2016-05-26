#pragma once

#include "dssi/plugin.h"
#include "state.h"
#include "util/buf.h"

template<typename T> class Node {

    static DSSI::cursor const INITIAL_CURSOR = 0;

    DSSI::cursor _cursor;

public:

    Node(char const *label, State&);

    virtual ~Node();

    void start() {
        _cursor = INITIAL_CURSOR;
        startImpl();
    }

    View<T> render(DSSI::cursor newCursor);

    DSSI::cursor cursor() {
        return _cursor;
    }

protected:

    Buffer<T> _buf;

    State& _state;

    virtual void startImpl() = 0;

    virtual void renderImpl() = 0;

};

#define NODE_INSTANTIATE(T) \
    template Node<T>::Node(char const *, State&); \
    template Node<T>::~Node(); \
    template View<T> Node<T>::render(DSSI::cursor);
