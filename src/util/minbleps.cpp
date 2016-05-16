#include "minbleps.h"

#include <boost/math/common_factor_rt.hpp>
#include <ladspa.h>
#include <cmath>
#include <complex>

#include "../config.h"
#include "buf.h"
#include "util.h"

MinBLEPs::MinBLEPs(Config const *config)
        : _scale((int) roundf(config->workingClock() / config->_pcmRate)) {
    // FIXME LATER: Do the right thing if these aren't integers.
    int const naiveRate = int(roundf(config->workingClock()));
    int const pcmRate = int(roundf(config->_pcmRate));
    int const minBlepCount = naiveRate / boost::math::gcd(naiveRate, pcmRate); // FIXME LATER: This could be huge.
    debug("Creating %d minBLEPs.", minBlepCount);
    // XXX: Use kaiser and/or satisfy min transition?
    // Closest even order to 4/transition:
    int const order = int(round(4 / config->_transition / 2)) * 2;
    int const kernelSize = order * minBlepCount + 1; // Odd.
    // The fft/ifft are too slow unless size is a power of 2:
    int fftSize = 2; // Can't be the trivial power as we need a midpoint.
    while (fftSize < kernelSize) {
        fftSize <<= 1;
    }
    int const midpoint = fftSize / 2; // Index of peak of sinc.
    // If cutoff is .5 the sinc starts and ends with zero.
    // The window is necessary for a reliable integral height later:
    Buffer<double> accumulator("accumulator", kernelSize);
    accumulator.blackman();
    {
        Buffer<double> sinc("sinc", kernelSize);
        for (int i = 0; i < kernelSize; ++i) {
            sinc.put(i, (double(i) / (kernelSize - 1) * 2 - 1) * order * config->_cutoff);
        }
        sinc.sinc();
        accumulator.mul(sinc.begin());
    }
    accumulator.mul(1. / minBlepCount * config->_cutoff * 2); // It's now a band-limited impulse (BLI).
    accumulator.pad((fftSize - kernelSize + 1) / 2, (fftSize - kernelSize - 1) / 2, 0);
    {
        Buffer<std::complex<double>> fftAppliance("fftAppliance", fftSize);
        fftAppliance.fill(accumulator.begin());
        fftAppliance.fft();
        // Everything is real after we discard the phase info here:
        accumulator.fillAbs(fftAppliance.begin());
        // The "real cepstrum" is symmetric apart from its first element:
        accumulator.add(1e-50); // Avoid taking log of zero. XXX: Why add not clamp?
        accumulator.ln();
        fftAppliance.fill(accumulator.begin());
        fftAppliance.ifft(); // It's now the real cepstrum.
        // Leave first point, zero max phase part, double min phase part to compensate.
        // The midpoint is shared between parts so it doesn't change:
        fftAppliance.mul(1, midpoint, std::complex<double>(2));
        fftAppliance.fill(midpoint + 1, fftSize, std::complex<double>(0));
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
    /*
     # The naiverate and outrate will line up at 1 second:
     dualscale = outrate // fractions.gcd(naiverate, outrate)
     nearest = np.arange(naiverate, dtype = np.int32) * dualscale
     self.naivex2outx = nearest // scale
     self.naivex2shape = self.naivex2outx * scale - nearest + scale - 1
     self.demultiplexed = np.empty(self.minblep.shape, dtype = self.minblep.dtype)
     for i in xrange(scale):
     self.demultiplexed[i * self.mixinsize:(i + 1) * self.mixinsize] = self.minblep[i::scale]
     self.naivex2off = self.naivex2shape * self.mixinsize
     self.outx2minnaivex = np.empty(outrate, dtype = self.naivex2outx.dtype)
     for naivex in xrange(naiverate - 1, -1, -1):
     self.outx2minnaivex[self.naivex2outx[naivex]] = naivex
     */
    debug("Finished creating minBLEPs.");
}

cursor_t MinBLEPs::getMinNaiveN(cursor_t naiveX, cursor_t pcmCount) const {
    return pcmCount * _scale; // FIXME: Do it properly.
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
