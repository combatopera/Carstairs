#include "minbleps.h"

#include <boost/math/common_factor_rt.hpp>
#include <ladspa.h>
#include <cmath>

#include "../config.h"
#include "buf.h"
#include "util.h"

MinBLEPs::MinBLEPs(Config const *config)
        : _scale((int) roundf(config->pluginClock() / config->_pcmRate)) {
    // FIXME LATER: Do the right thing if these aren't integers.
    int naiveRate = int(roundf(config->pluginClock()));
    int pcmRate = int(roundf(config->_pcmRate));
    int minBlepCount = naiveRate / boost::math::gcd(naiveRate, pcmRate); // FIXME LATER: This could be huge.
    debug("Creating %d minBLEPs.", minBlepCount);
    // XXX: Use kaiser and/or satisfy min transition?
    // Closest even order to 4/transition:
    int order = int(round(4 / config->_transition / 2)) * 2;
    int kernelSize = order * minBlepCount + 1;
    // The fft/ifft are too slow unless size is a power of 2:
    int size = 2; // Can't be the trivial power as we need a midpoint.
    while (size < kernelSize) {
        size <<= 1;
    }
    int midpoint = size / 2; // Index of peak of sinc.
    // If cutoff is .5 the sinc starts and ends with zero.
    // The window is necessary for a reliable integral height later:
    Buffer<double> bli("bli", kernelSize);
    bli.blackman();
    {
        Buffer<double> x("x", kernelSize);
        for (int i = 0; i < kernelSize; ++i) {
            x.put(i, (double(i) / (kernelSize - 1) * 2 - 1) * order * config->_cutoff);
        }
        x.sinc();
        bli.mul(x.begin());
    }
    bli.mul(1. / minBlepCount * config->_cutoff * 2);
    bli.zeroPad((size - kernelSize + 1) / 2, (size - kernelSize - 1) / 2);
    // Everything is real after we discard the phase info here:
    /*
     absdft = np.abs(np.fft.fft(self.bli))
     # The "real cepstrum" is symmetric apart from its first element:
     realcepstrum = np.fft.ifft(np.log(np.maximum(self.minmag, absdft)))
     # Leave first point, zero max phase part, double min phase part to compensate.
     # The midpoint is shared between parts so it doesn't change:
     realcepstrum[1:midpoint] *= 2
     realcepstrum[midpoint + 1:] = 0
     self.minbli = np.fft.ifft(np.exp(np.fft.fft(realcepstrum))).real
     self.minblep = np.cumsum(self.minbli, dtype = floatdtype)
     # Prepend zeros to simplify naivex2outx calc:
     self.minblep = np.append(np.zeros(scale - 1, floatdtype), self.minblep)
     # Append ones so that all mixins have the same length:
     ones = (-len(self.minblep)) % scale
     self.minblep = np.append(self.minblep, np.ones(ones, floatdtype))
     self.mixinsize = len(self.minblep) // scale
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
