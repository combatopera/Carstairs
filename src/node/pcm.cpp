#include "pcm.h"

PCM::PCM(Config const& config, State& state, Node<float>& naive)
        : Node("PCM", state), _minBLEPs(config), _naive(naive), _dc(INITIAL_DC), _overflowIndex(0) {
    // Nothing else.
}

void PCM::resetImpl() {
    _dc = INITIAL_DC;
    _overflowIndex = 0;
    _target.setLimit(0);
}

void PCM::renderImpl() {
    auto pcmRef = cursor();
    auto pcmCount = _buf.limit();
    auto naiveRef = _naive.cursor();
    auto naive = _naive.render(_minBLEPs.pcmXToNaiveX(pcmRef + pcmCount));
    auto naiveN = naive.limit();
    _derivative.snapshot(naive);
    _derivative.differentiate(_dc);
    _minBLEPs.pastePrepare(naiveRef + naiveN - 1, pcmRef);
    auto overflowSize = _target.limit() - _overflowIndex;
    _target.fill(0, overflowSize, _target.begin(_overflowIndex));
    _target.setLimit(_minBLEPs.targetLimit());
    _target.fill(overflowSize, _target.limit(), _dc);
    for (unsigned i = 0; i < naiveN; ++i) {
        auto amp = _derivative.at(i);
        if (amp) {
            _minBLEPs.pastePrepare(naiveRef + i, pcmRef);
            _minBLEPs.pastePerform(amp, _target);
        }
    }
    _buf.fill(_target.begin());
    _overflowIndex = pcmCount;
    if (naiveN) { // Otherwise _dc doesn't change.
        _dc = naive.at(naiveN - 1);
    }
}
