#pragma once

#include <ladspa.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "../util/minbleps.h"
#include "../util/paster.h"

class PCM: public Node<LADSPA_Data> {

    MinBLEPs const _minBLEPs;

    Paster _paster;

    Node<float>& _naive;

    Buffer<float> _derivative {"_derivative"}, _pcm {"_pcm"};

    sizex _overflowCount;

    float _dc;

    void startImpl() {
        _overflowCount = 0;
        _dc = 0;
    }

    void renderImpl() {
        auto const pcmRef = cursor();
        auto const pcmCount = _buf.limit();
        auto const naiveRef = _naive.cursor();
        auto const naive = _naive.renderNew(_minBLEPs.pcmXToNaiveX(pcmRef + pcmCount));
        auto const naiveCount = naive.limit();
        auto const pcmLimit = _paster.pcmCountWithOverflow(naiveRef + naiveCount - 1, pcmRef);
        _derivative.snapshot(naive);
        _derivative.differentiate(_dc);
        _pcm.setLimit(pcmLimit);
        _paster.pasteMulti(_derivative, naiveRef, pcmRef, _pcm, _overflowCount, _dc);
        _buf.fill(_pcm.begin());
        _pcm.fill(0, _overflowCount = pcmLimit - pcmCount, _pcm.begin(pcmCount));
        if (naiveCount) { // Otherwise _dc doesn't change.
            _dc = naive.at(naiveCount - 1);
        }
    }

public:

    PCM(Config const& config, State& state, Node<float>& naive, int pcmRate)
            : Node("PCM", state), _minBLEPs(config, pcmRate), _paster(_minBLEPs), _naive(naive), //
            _overflowCount(), _dc() {
    }

};
