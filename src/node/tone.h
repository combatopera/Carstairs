#pragma once

#include "../node.h"

class Tone: public Node<int> {

    void renderImpl(unsigned long n);

};
