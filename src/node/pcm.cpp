#include "pcm.h"

PCM::PCM(Config const& config, State& state, Node<float>& naive, int pcmRate)
        : Node("PCM", state), _minBLEPs(config, pcmRate), _paster(_minBLEPs), _naive(naive), _dc(INITIAL_DC), _overflowIndex(0) {
    // Nothing else.
}

void PCM::resetImpl() {
    _dc = INITIAL_DC;
    _overflowIndex = 0;
    _pcm.setLimit(0); // Clear any overflow.
}

void PCM::renderImpl() {
    auto const pcmRef = cursor();
    auto const pcmCount = _buf.limit();
    auto const naiveRef = _naive.cursor();
    auto const naive = _naive.render(_minBLEPs.pcmXToNaiveX(pcmRef + pcmCount));
    auto const naiveCount = naive.limit();
    _derivative.snapshot(naive);
    _derivative.differentiate(_dc);
    _paster.pastePrepare(naiveRef + naiveCount - 1, pcmRef);
    auto const overflowCount = _pcm.limit() - _overflowIndex;
    _pcm.fill(0, overflowCount, _pcm.begin(_overflowIndex));
    _pcm.setLimit(_paster.pcmCountWithOverflow());
    _pcm.fill(overflowCount, _pcm.limit(), _dc);
    for (sizex i = 0; i < naiveCount; ++i) {
        auto const amp = _derivative.at(i);
        if (amp) {
            _paster.pastePrepare(naiveRef + i, pcmRef);
            _paster.pastePerform(amp, _pcm);
        }
    }
    _buf.fill(_pcm.begin());
    _overflowIndex = pcmCount;
    if (naiveCount) { // Otherwise _dc doesn't change.
        _dc = naive.at(naiveCount - 1);
    }
}
