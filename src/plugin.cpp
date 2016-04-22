#include <dssi.h>
#include <ladspa.h>

#include "dizzYM.h"

const char * const dizzYM::PortNames[PortCount] = { //
        "Output", //
                "Sustain (on/off)", //
        };

const LADSPA_PortDescriptor dizzYM::PortDescriptors[PortCount] = { //
        LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO, //
                LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL, //
        };

const LADSPA_PortRangeHint dizzYM::PortRangeHints[PortCount] = { //
        {0, 0, 0}, //
                {LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1}, //
        };

const LADSPA_Properties dizzYM::Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;

const LADSPA_Descriptor dizzYM::ladspaDescriptor = { //
        0, // UniqueID
                "dizzYM", // Label
                Properties, //
                "YM2149", // Name
                "Andrzej Cichocki", // Maker
                "Andrzej Cichocki", // Copyright
                PortCount, //
                PortDescriptors, //
                PortNames, //
                PortRangeHints, //
                0, // ImplementationData
                instantiate, //
                connect_port, //
                activate, //
                run, //
                0, // run_adding
                0, // set_run_adding_gain
                deactivate, //
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
