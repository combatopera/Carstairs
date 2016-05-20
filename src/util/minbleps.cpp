#include "minbleps.h"

#include <boost/math/common_factor_rt.hpp>
#include <ladspa.h>
#include <cmath>
#include <complex>

#include "../config.h"
#include "buf.h"
#include "util.h"

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
    int const evenOrder = config->evenEmpiricalOrder();
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
    accumulator.integrate(); // It's now minBlepCount interleaved minBLEPs!
    _minBLEPs.setLimit(accumulator.limit());
    _minBLEPs.fillNarrowing(accumulator.begin());
    debug("Finished creating minBLEPs.");
}

void MinBLEPs::paste(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const {
    for (DSSI::cursor pcmI = 0; pcmI < pcmBuf.limit(); ++pcmI) {
        LADSPA_Data acc = 0;
        for (int s = 0; s < _scale; ++s) {
            acc += naiveBuf.at(pcmI * _scale + s);
        }
        pcmBuf.put(pcmI, acc / float(_scale));
    }
}

void MinBLEPs::paste2(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const {
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
