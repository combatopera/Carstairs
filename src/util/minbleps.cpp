#include "minbleps.h"

#include <cassert>

#include "util.h"

namespace {

Log const LOG(__FILE__);

sizex getEvenFftSize(sizex const minSize) {
    auto evenFftSize = sizex(2); // Smallest even power of 2.
    while (evenFftSize < minSize) {
        evenFftSize <<= 1;
    }
    return evenFftSize;
}

}

MinBLEPs::MinBLEPs(Config const& config, int pcmRate)
        : _pcmRate(pcmRate), _naiveRate(config.naiveRate()), _minBLEPCount(config._minBLEPCount) {
    debug("Creating %u minBLEPs.", _minBLEPCount);
    auto const evenOrder = config.evenEmpiricalOrder();
    debug("For transition %.3f/%d using filter order: %d", config._transition, pcmRate, evenOrder);
    auto const oddKernelSize = evenOrder * _minBLEPCount + 1;
    // Use a power of 2 for fastest fft/ifft, and can't be trivial power as we need a midpoint:
    auto const evenFftSize = getEvenFftSize(oddKernelSize);
    // If cutoff is .5 the sinc starts and ends with zero.
    // The window is necessary for a reliable integral height later:
    Buffer<double> accumulator("accumulator", oddKernelSize);
    accumulator.blackman();
    {
        Buffer<double> sinc("sinc", oddKernelSize);
        auto const uniqueLimit = (oddKernelSize + 1) / 2;
        for (auto i = uniqueLimit - 1; SIZEX_NEG != i; --i) {
            sinc.put(i, (double(i) / (oddKernelSize - 1) * 2 - 1) * evenOrder * config.cutoff());
        }
        assert(!sinc.at(uniqueLimit - 1));
        sinc.mirror(); // Logically values should be negated, but doesn't matter because sinc symmetric.
        sinc.sinc();
        accumulator.mul(sinc.begin());
    }
    accumulator.mul(1. / _minBLEPCount * config.cutoff() * 2); // It's now a band-limited impulse (BLI).
#ifdef DIZZYM_UNIT_TEST
    _BLI.snapshot(accumulator);
#endif
    accumulator.pad((evenFftSize - oddKernelSize + 1) / 2, (evenFftSize - oddKernelSize - 1) / 2, 0);
    assert(accumulator.limit() == evenFftSize);
    {
        Buffer<std::complex<double>> fftAppliance("fftAppliance");
        accumulator.rceps(fftAppliance, config._rcepsAddBeforeLog);
#ifdef DIZZYM_UNIT_TEST
        _realCepstrum.snapshot(fftAppliance);
#endif
        accumulator.minPhaseFromRceps(fftAppliance); // It's now a min-phase BLI.
    }
    accumulator.integrate(); // It's now _minBLEPCount interleaved minBLEPs!
    _minBLEPs.setLimit(accumulator.limit());
    _minBLEPs.fillNarrowing(accumulator.begin());
    debug("Finished creating minBLEPs.");
}
