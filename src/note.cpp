#include "note.h"

void Note::reset() {
    _on = _off = -1;
    _velocity = 0;
}

void Note::on(unsigned long on, int velocity) {
    _on = on;
    _off = -1;
    _velocity = velocity;
}

bool Note::isActive() {
    return _on >= 0;
}
