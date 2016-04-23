#include <dssi.h>
#include <ladspa.h>

#include "dizzYM.h"
#include "port.h"

LADSPA_PortDescriptor *getPortDescriptors() {
    LADSPA_PortDescriptor *PortDescriptors = new LADSPA_PortDescriptor[dizzYM::PortCount];
    for (int i = 0; i < dizzYM::PortCount; ++i) {
        PortDescriptors[i] = dizzYM::PORTS[i]._descriptor;
    }
    return PortDescriptors;
}

const LADSPA_PortRangeHint dizzYM::PortRangeHints[PortCount] = { //
        {0, 0, 0}, //
                {LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1}, //
        };

const LADSPA_Properties dizzYM::Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;

const char **getPortNames() {
    const char **PortNames = new const char *[dizzYM::PortCount];
    for (int i = 0; i < dizzYM::PortCount; ++i) {
        PortNames[i] = dizzYM::PORTS[i]._name;
    }
    return PortNames;
}

const LADSPA_Descriptor dizzYM::ladspaDescriptor = { //
        0, // UniqueID
                "dizzYM", // Label
                Properties, //
                "YM2149", // Name
                "Andrzej Cichocki", // Maker
                "Andrzej Cichocki", // Copyright
                PortCount, //
                getPortDescriptors(), //
                getPortNames(), //
                PortRangeHints, //
                0, // ImplementationData
                instantiate, //
                connect_port, //
                activate, //
                run, //
                0, // run_adding
                0, // set_run_adding_gain
                0, // deactivate
                cleanup, //
        };

const DSSI_Descriptor dizzYM::dssiDescriptor = { //
        1, // API version, must be 1.
                &ladspaDescriptor, //
                0, // configure()
                0, // get_program()
                0, // select_program()
                get_midi_controller_for_port, //
                run_synth, //
                0, // run_synth_adding()
                0, // run_multiple_synths()
                0, // run_multiple_synths_adding()
        };

extern "C" {

const DSSI_Descriptor *dssi_descriptor(unsigned long index) {
    if (index == 0) {
        return &dizzYM::dssiDescriptor;
    }
    else {
        return 0;
    }
}

}
