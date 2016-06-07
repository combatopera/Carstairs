#include "node.h"

#include <ladspa.h>

template<typename T> Node<T>::Node(char const *label, State const& state)
        : _cursor(-1), _buf(label), _state(state) {
    // Nothing else.
}

template<typename T> Node<T>::~Node() {
    // Do nothing.
}

template<typename T> View<T> Node<T>::render(DSSI::cursor newCursor) {
    if (_cursor < newCursor) {
        _buf.setLimit(sizex(newCursor - _cursor));
        renderImpl();
        _cursor = newCursor;
    }
    return _buf; // TODO: Enforce return of current buf when nothing to render.
}

CARSTAIRS_NODE_INSTANTIATE(int)
CARSTAIRS_NODE_INSTANTIATE(LADSPA_Data)
