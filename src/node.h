// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

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

    DSSI::cursor _cursor = -1;

    inline void catchUpImpl(DSSI::cursor newCursor, bool makeData) {
        if (_cursor < newCursor) {
            _buf.setLimit(sizex(newCursor - _cursor));
            renderImpl(); // TODO LATER: Only actually make the data in makeData case.
            _cursor = newCursor;
        }
    }

public:

    Node(char const *label, State const& state)
            : _buf(label), _state(state) {
        // Nothing else.
    }

    virtual ~Node() {
        // Do nothing.
    }

    void start() {
        _cursor = 0;
        startImpl();
    }

    void catchUp(DSSI::cursor newCursor) {
        catchUpImpl(newCursor, false);
    }

    View<T> render(DSSI::cursor newCursor) {
        catchUpImpl(newCursor, true);
        return _buf; // TODO: Enforce return of current buf when nothing to render.
    }

    View<T> renderNew(DSSI::cursor const newCursor) {
        if (newCursor == _cursor) {
            _buf.setLimit(0);
            return _buf;
        }
        return render(newCursor);
    }

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
