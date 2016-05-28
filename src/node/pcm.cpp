#include "pcm.h"

PCM::PCM(Config const& config, State& state, Node<float>& naive, int pcmRate)
        : Node("PCM", state), _minBLEPs(config, pcmRate), _paster(_minBLEPs), _naive(naive), //
        _overflowCount(), _dc() {
}

void PCM::startImpl() {
    _overflowCount = 0;
    _dc = 0;
}

void PCM::renderImpl() {
    auto const pcmRef = cursor();
    auto const pcmCount = _buf.limit();
    auto const naiveRef = _naive.cursor();
    auto const naive = _naive.render(_minBLEPs.pcmXToNaiveX(pcmRef + pcmCount));
    auto const naiveCount = naive.limit();
    auto const pcmLimit = _paster.pcmCountWithOverflow(naiveRef + naiveCount - 1, pcmRef);
    _derivative.snapshot(naive);
    _derivative.differentiate(_dc);
    _pcm.setLimit(pcmLimit);
    _pcm.fill(_overflowCount, pcmLimit, _dc);
    _paster.pasteMulti(_derivative, naiveRef, pcmRef, _pcm);
    _buf.fill(_pcm.begin());
    _pcm.fill(0, _overflowCount = pcmLimit - pcmCount, _pcm.begin(pcmCount));
    if (naiveCount) { // Otherwise _dc doesn't change.
        _dc = naive.at(naiveCount - 1);
    }
}
