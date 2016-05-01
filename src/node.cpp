#include "node.h"

#include <ladspa.h>

#include "state.h"
#include "util.h"

template<typename T> Node<T>::Node(State *state)
        : _state(state), _cursor(0) {
    // Nothing else.
}

template<typename T> Node<T>::~Node() {
    // Do nothing.
}

template<typename T> View<T> Node<T>::render(unsigned long newCursor) {
    if (_cursor < newCursor) {
        unsigned long blockSize = newCursor - _cursor;
        _buf.setLimit(blockSize);
        renderImpl();
        _cursor = newCursor;
    }
    return _buf;
}

NODE_INSTANTIATE(int)
NODE_INSTANTIATE(LADSPA_Data)
