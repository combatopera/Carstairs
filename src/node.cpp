#include "node.h"

#include <ladspa.h>

template<typename T> Node<T>::Node(char const *label, State& state)
        : _cursor(INITIAL_CURSOR), _buf(label), _state(state) {
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
    return _buf;
}

NODE_INSTANTIATE(int)
NODE_INSTANTIATE(LADSPA_Data)
