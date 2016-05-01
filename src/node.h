#pragma once

#include <ladspa.h>
#include <stddef.h>

template<typename T> class Node {

    unsigned long _cursor;

    size_t _capacity;

public:

    Node();

    virtual ~Node();

    virtual T *render(unsigned long newCursor);

protected:

    T *_buffer;

    virtual void renderImpl(T *_buffer, unsigned long n) = 0;

};

class Tone: public Node<int> {

    void renderImpl(int*buffer, unsigned long n);

public:

    Tone();

    ~Tone();

    int *render(unsigned long newCursor);

};

class PCM: public Node<LADSPA_Data> {

    Tone _tone;

    void renderImpl(LADSPA_Data*buffer, unsigned long n);

public:

    PCM();

    ~PCM();

    LADSPA_Data *render(unsigned long newCursor);

};
