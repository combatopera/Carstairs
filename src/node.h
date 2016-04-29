#include <ladspa.h>
#include <stddef.h>

template<typename T>
class Node {

    unsigned long _cursor;

    T *_buffer;

    size_t _capacity;

public:

    Node();

    virtual ~Node();

    T *render(unsigned long newCursor);

    virtual void renderImpl(T *buffer, unsigned long n);

};

class Tone: public Node<int> {

    void renderImpl(int *buffer, unsigned long n);

};

class PCM: public Node<LADSPA_Data> {

    void renderImpl(LADSPA_Data *buffer, unsigned long n);

};
