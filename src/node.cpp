#include "node.h"

#include <ladspa.h>
#include <stdlib.h>

#include "state.h"

template<typename T> Node<T>::Node(State *state)
        : _state(state), _cursor(0) {
    _buffer = (T *) malloc(_capacity = 0);
}

template<typename T> Node<T>::~Node() {
    free(_buffer);
}

template<typename T> T *Node<T>::render(unsigned long newCursor) {
    if (_cursor < newCursor) {
        unsigned long blockSize = newCursor - _cursor;
        _cursor = newCursor;
        if (blockSize > _capacity) {
            _buffer = (T *) realloc(_buffer, _capacity = (blockSize * sizeof *_buffer));
        }
        renderImpl(blockSize);
    }
    return _buffer;
}

NODE_INSTANTIATE(int)
NODE_INSTANTIATE(LADSPA_Data)
