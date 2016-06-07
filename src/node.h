#pragma once

#include "dssi/plugin.h"
#include "state.h"
#include "util/buf.h"

template<typename T> class Node {

    DSSI::cursor _cursor;

public:

    Node(char const *label, State const&);

    virtual ~Node();

    void start() {
        _cursor = 0;
        startImpl();
    }

    View<T> render(DSSI::cursor newCursor);

    DSSI::cursor cursor() {
        return _cursor;
    }

protected:

    Buffer<T> _buf;

    State const& _state;

    virtual void startImpl() = 0;

    virtual void renderImpl() = 0;

};

#define CARSTAIRS_NODE_INSTANTIATE(T) \
    template Node<T>::Node(char const *, State const&); \
    template Node<T>::~Node(); \
    template View<T> Node<T>::render(DSSI::cursor);
