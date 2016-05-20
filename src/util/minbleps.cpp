#include "minbleps.h"

#include <boost/math/common_factor_rt.hpp>
#include <ladspa.h>
#include <cmath>
#include <complex>

#include "../config.h"
#include "buf.h"
#include "util.h"

static int closestEven(int n) {
    return int(round(n * .5)) * 2;
}

static int getEvenFftSize(int minSize) {
    int evenFftSize = 2; // Smallest even power of 2.
    while (evenFftSize < minSize) {
        evenFftSize <<= 1;
    }
    return evenFftSize;
}

MinBLEPs::MinBLEPs(Config const *config)
        : _scale((int) roundf(float(config->naiveRate()) / float(config->_pcmRate))) {
    _naiveRate = config->naiveRate();
    _pcmRate = config->_pcmRate;
    int const minBlepCount = _naiveRate / boost::math::gcd(_naiveRate, _pcmRate); // FIXME LATER: This could be huge.
    debug("Creating %d minBLEPs.", minBlepCount);
    int const evenOrder = closestEven(config->empiricalOrder());
    int const oddKernelSize = evenOrder * minBlepCount + 1; // Odd.
    // Use a power of 2 for fastest fft/ifft, and can't be trivial power as we need a midpoint:
    int const evenFftSize = getEvenFftSize(oddKernelSize);
    int const fftMidpoint = evenFftSize / 2; // Index of peak of sinc.
    // If cutoff is .5 the sinc starts and ends with zero.
    // The window is necessary for a reliable integral height later:
    Buffer<double> accumulator("accumulator", oddKernelSize);
    accumulator.blackman();
    {
        Buffer<double> sinc("sinc", oddKernelSize);
        for (int i = 0; i < oddKernelSize; ++i) {
            sinc.put(i, (double(i) / (oddKernelSize - 1) * 2 - 1) * evenOrder * config->_cutoff);
        }
        sinc.sinc();
        accumulator.mul(sinc.begin());
    }
    accumulator.mul(1. / minBlepCount * config->_cutoff * 2); // It's now a band-limited impulse (BLI).
    accumulator.pad((evenFftSize - oddKernelSize + 1) / 2, (evenFftSize - oddKernelSize - 1) / 2, 0);
    {
        Buffer<std::complex<double>> fftAppliance("fftAppliance", evenFftSize);
        fftAppliance.fillWidening(accumulator.begin());
        fftAppliance.fft();
        // Everything is real after we discard the phase info here:
        accumulator.fillAbs(fftAppliance.begin());
        // The "real cepstrum" is symmetric apart from its first element:
        accumulator.add(1e-50); // Avoid taking log of zero. XXX: Why add not clamp?
        accumulator.ln();
        fftAppliance.fillWidening(accumulator.begin());
        fftAppliance.ifft(); // It's now the real cepstrum.
        // Leave first point, zero max phase part, double min phase part to compensate.
        // The midpoint is shared between parts so it doesn't change:
        fftAppliance.mul(1, fftMidpoint, std::complex<double>(2));
        fftAppliance.fill(fftMidpoint + 1, evenFftSize, std::complex<double>(0));
        fftAppliance.fft();
        fftAppliance.exp();
        fftAppliance.ifft();
        accumulator.fillReal(fftAppliance.begin()); // It's now a min-phase BLI.
    }
    accumulator.integrate(); // It's now a minBLEP!
    // Prepend zeros to simplify naivex2outx calc:
    accumulator.pad(minBlepCount - 1, 0, 0);
    // Append ones so that all mixins have the same length:
    int const mixinSize = (int(accumulator.limit()) + minBlepCount - 1) / minBlepCount;
    accumulator.pad(0, mixinSize * minBlepCount - accumulator.limit(), 1);
    // The naiverate and outrate will line up at 1 second:
    int const dualScale = _pcmRate / boost::math::gcd(_naiveRate, _pcmRate);
    _naiveXToPcmX.setLimit(_naiveRate);
    for (int i = 0; i < _naiveRate; ++i) {
        _naiveXToPcmX.put(i, i * dualScale / minBlepCount);
    }
    Buffer<int> naivex2shape("naivex2shape", _naiveRate);
    for (int i = 0; i < _naiveRate; ++i) {
        naivex2shape.put(i, _naiveXToPcmX.at(i) * minBlepCount - i * dualScale + minBlepCount - 1);
    }
    Buffer<double> demultiplexed("demultiplexed", mixinSize * minBlepCount);
    for (int i = 0; i < minBlepCount; ++i) {
        for (int j = 0; j < mixinSize; ++j) {
            demultiplexed.put(i * mixinSize + j, accumulator.at(i + minBlepCount * j));
        }
    }
    Buffer<int> naivex2off("naivex2off", _naiveRate);
    for (int i = 0; i < _naiveRate; ++i) {
        naivex2off.put(i, naivex2shape.at(i) * mixinSize);
    }
    _pcmXToMinNaiveX.setLimit(_pcmRate);
    for (int naivex = _naiveRate - 1; naivex >= 0; --naivex) {
        _pcmXToMinNaiveX.put(_naiveXToPcmX.at(naivex), naivex);
    }
    debug("Finished creating minBLEPs.");
}

cursor_t MinBLEPs::getMinNaiveN(cursor_t naiveX, cursor_t pcmCount) const {
    return pcmCount * _scale; // FIXME: Do it properly.
}

cursor_t MinBLEPs::getMinNaiveN2(cursor_t naiveX, cursor_t pcmCount) const {
    cursor_t pcmX = _naiveXToPcmX.at(naiveX) + pcmCount;
    cursor_t shift = pcmX / _pcmRate;
    pcmX -= _pcmRate * shift;
    naiveX -= _naiveRate * shift;
    return _pcmXToMinNaiveX.at(pcmX) - naiveX;
}

void MinBLEPs::paste(cursor_t naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const {
    for (cursor_t pcmI = 0; pcmI < pcmBuf.limit(); ++pcmI) {
        LADSPA_Data acc = 0;
        for (int s = 0; s < _scale; ++s) {
            acc += naiveBuf.at(pcmI * _scale + s);
        }
        pcmBuf.put(pcmI, acc / float(_scale));
    }
}

void MinBLEPs::paste2(cursor_t naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const {
//    out0 = naivex2outxp[naivex]
//    dclevel = 0
//    dcindex = 0
//    while ampsize:
//      ampchunk = min(ampsize, naiverate - naivex)
//      for naivex in xrange(naivex, naivex + ampchunk):
//        a = ampp[0]
//        ampp += 1
//        if a:
//          i = naivex2outxp[naivex] - out0
//          mixinp = demultiplexedp + naivex2offp[naivex]
//          if dcindex <= i: # We can DC-adjust while pasting this mixin.
//            dccount = i - dcindex
//            for UNROLL in xrange(dccount):
//              outp[0] += dclevel
//              outp += 1
//            for UNROLL in xrange(mixinsize):
//              outp[0] += mixinp[0] * a + dclevel
//              outp += 1
//              mixinp += 1
//          else: # The mixin starts before the pending DC adjustment.
//            dccount = i + mixinsize - dcindex
//            for UNROLL in xrange(dccount):
//              outp[0] += dclevel
//              outp += 1
//            outp -= mixinsize
//            for UNROLL in xrange(mixinsize):
//              outp[0] += mixinp[0] * a
//              outp += 1
//              mixinp += 1
//          dcindex = i + mixinsize
//          dclevel += a
//      ampsize -= ampchunk
//      naivex = 0
//      out0 -= outrate
//    dccount = outsize - dcindex
//    for UNROLL in xrange(dccount):
//      outp[0] += dclevel
//      outp += 1
}
