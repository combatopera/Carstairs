#pragma once

#include "dssi/plugin.h"
#include "state.h"
#include "util/buf.h"

class Maskable {

public:

    virtual ~Maskable() {
    }

    virtual void catchUp(DSSI::cursor) = 0;

};

template<typename T> class Node: public Maskable {

    DSSI::cursor _cursor;

public:

    Node(char const *label, State const&);

    virtual ~Node();

    void start() {
        _cursor = 0;
        startImpl();
    }

    void catchUp(DSSI::cursor newCursor) {
        render(newCursor); // TODO LATER: No need to actually make the data in this case.
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
