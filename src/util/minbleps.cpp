#include "minbleps.h"

#include <ladspa.h>
#include <cmath>

#include "../buf.h"
#include "../config.h"
#include "util.h"

MinBLEPs::MinBLEPs(Config const *config)
        : _scale((int) roundf(config->pluginClock() / config->_pcmRate)) {
/*
        log.debug('Creating minBLEPs.')
        # XXX: Use kaiser and/or satisfy min transition?
        # Closest even order to 4/transition:
        order = int(self.round(4 / transition / 2)) * 2
        kernelsize = order * scale + 1
        # The fft/ifft are too slow unless size is a power of 2:
        size = 2 ** 0
        while size < kernelsize:
          size <<= 1
        midpoint = size // 2 # Index of peak of sinc.
        x = (np.arange(kernelsize) / (kernelsize - 1) * 2 - 1) * order * cutoff
        # If cutoff is .5 the sinc starts and ends with zero.
        # The window is necessary for a reliable integral height later:
        self.bli = np.blackman(kernelsize) * np.sinc(x) / scale * cutoff * 2
        rpad = (size - kernelsize) // 2 # Observe floor of odd difference.
        lpad = 1 + rpad
        self.bli = np.concatenate([np.zeros(lpad), self.bli, np.zeros(rpad)])
        # Everything is real after we discard the phase info here:
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
        log.debug('%s minBLEPs created.', scale)
*/
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
