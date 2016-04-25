#pragma once

class Note {

    friend class dizzYM;

    long _on, _off;

    int _velocity;

    void reset();

    void on(unsigned long on, int velocity);

    bool isActive();

};
