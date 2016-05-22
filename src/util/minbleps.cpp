#include "minbleps.h"

static int getEvenFftSize(int minSize) {
    int evenFftSize = 2; // Smallest even power of 2.
    while (evenFftSize < minSize) {
        evenFftSize <<= 1;
    }
    return evenFftSize;
}

MinBLEPs::MinBLEPs(Config const *config)
        : _pcmRate(config->_pcmRate), _naiveRate(config->naiveRate()), _minBLEPCount(config->_minBLEPCount) {
    debug("Creating %d minBLEPs.", _minBLEPCount);
    int const evenOrder = config->evenEmpiricalOrder();
    int const oddKernelSize = evenOrder * _minBLEPCount + 1; // Odd.
    // Use a power of 2 for fastest fft/ifft, and can't be trivial power as we need a midpoint:
    int const evenFftSize = getEvenFftSize(oddKernelSize);
    int const fftMidpoint = evenFftSize / 2; // Index of peak of sinc after padding.
    // If cutoff is .5 the sinc starts and ends with zero.
    // The window is necessary for a reliable integral height later:
    Buffer<double> accumulator("accumulator", oddKernelSize);
    accumulator.blackman();
    {
        Buffer<double> sinc("sinc", oddKernelSize);
        int const uniqueLimit = (oddKernelSize + 1) / 2;
        for (int i = 0; i < uniqueLimit; ++i) {
            sinc.put(i, (double(i) / (oddKernelSize - 1) * 2 - 1) * evenOrder * config->_cutoff);
        }
        assert(!sinc.at(uniqueLimit - 1));
        sinc.mirror(); // Logically values should be negated, but doesn't matter because sinc symmetric.
        sinc.sinc();
        accumulator.mul(sinc.begin());
    }
    accumulator.mul(1. / _minBLEPCount * config->_cutoff * 2); // It's now a band-limited impulse (BLI).
#ifdef DIZZYM_UNIT_TEST
    _BLI.snapshot(accumulator);
#endif
    accumulator.pad((evenFftSize - oddKernelSize + 1) / 2, (evenFftSize - oddKernelSize - 1) / 2, 0);
    assert(int(accumulator.limit()) == evenFftSize);
    {
        Buffer<std::complex<double>> fftAppliance("fftAppliance");
        accumulator.rceps(fftAppliance, config->_rcepsAddBeforeLog);
#ifdef DIZZYM_UNIT_TEST
        _realCepstrum.snapshot(fftAppliance);
#endif
        // Leave first point, zero max phase part, double min phase part to compensate.
        // The midpoint is shared between parts so it doesn't change:
        accumulator.mul(1, fftMidpoint, 2);
        accumulator.fill(fftMidpoint + 1, evenFftSize, 0);
        fftAppliance.fillWidening(accumulator.begin());
        fftAppliance.fft();
        fftAppliance.exp();
        fftAppliance.ifft();
        accumulator.fillReal(fftAppliance.begin()); // It's now a min-phase BLI.
    }
    accumulator.integrate(); // It's now _minBLEPCount interleaved minBLEPs!
    _minBLEPs.setLimit(accumulator.limit());
    _minBLEPs.fillNarrowing(accumulator.begin());
    debug("Finished creating minBLEPs.");
}
