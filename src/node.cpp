#include "node.h"

#include <ladspa.h>
#include <stdlib.h>

#include "util.h"

template<typename T> Node<T>::Node()
        : _cursor(0) {
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

void Tone::renderImpl(unsigned long n) {
    zero(_buffer, n);
}

void PCM::renderImpl(unsigned long n) {
    zero(_buffer, n);
}

template Node<int>::Node();
template Node<LADSPA_Data>::Node();
