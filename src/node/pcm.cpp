#include "pcm.h"

#include <ladspa.h>
#include <stddef.h>

#include "../enum.h"
#include "../state.h"
#include "../util.h"

PCM::PCM(State *state)
        : Node("PCM", state), _tone(state) {
    // Nothing else.
}

void PCM::renderImpl() {
    size_t n = _buf.limit();
    View<int> view = _tone.render(_cursor + n); // FIXME: Convert to chip time.
    for (index_t i = 0; i < n; ++i) {
        _buf.put(i, (LADSPA_Data) view.at(i));
    }
}
