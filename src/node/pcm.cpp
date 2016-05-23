#include "pcm.h"

PCM::PCM(Config const& config, State& state, Node<float>& naive)
        : Node("PCM", state), _minBLEPs(config), _naive(naive), _dc(INITIAL_DC), _overflowIndex(0) {
    // Nothing else.
}

void PCM::resetImpl() {
    _dc = INITIAL_DC;
    _overflowIndex = 0;
    _pcm.setLimit(0); // Clear any overflow.
}

void PCM::renderImpl() {
    auto pcmRef = cursor();
    auto pcmCount = _buf.limit();
    auto naiveRef = _naive.cursor();
    auto naive = _naive.render(_minBLEPs.pcmXToNaiveX(pcmRef + pcmCount));
    auto naiveCount = naive.limit();
    _derivative.snapshot(naive);
    _derivative.differentiate(_dc);
    _minBLEPs.pastePrepare(naiveRef + naiveCount - 1, pcmRef);
    auto overflowCount = _pcm.limit() - _overflowIndex;
    _pcm.fill(0, overflowCount, _pcm.begin(_overflowIndex));
    _pcm.setLimit(_minBLEPs.pcmCountWithOverflow());
    _pcm.fill(overflowCount, _pcm.limit(), _dc);
    for (index_t i = 0; i < naiveCount; ++i) {
        auto amp = _derivative.at(i);
        if (amp) {
            _minBLEPs.pastePrepare(naiveRef + i, pcmRef);
            _minBLEPs.pastePerform(amp, _pcm);
        }
    }
    _buf.fill(_pcm.begin());
    _overflowIndex = pcmCount;
    if (naiveCount) { // Otherwise _dc doesn't change.
        _dc = naive.at(naiveCount - 1);
    }
}
