#include "plugin.h"

#include <ladspa.h>
#include <stddef.h>
#include <array>

#include "dizzYM.h"

Descriptor::Descriptor() {
    _PortDescriptors = new LADSPA_PortDescriptor[PortCount];
    _PortNames = new const char *[PortCount];
    _PortRangeHints = new LADSPA_PortRangeHint[PortCount];
    for (unsigned i = 0; i < PortCount; ++i) {
        _PortDescriptors[i] = PORT_INFOS[i]._descriptor;
        _PortNames[i] = PORT_INFOS[i]._name;
        _PortRangeHints[i] = PORT_INFOS[i]._rangeHint; // Copy.
    }
    _ladspaDescriptor = { //
        0,// UniqueID
        "dizzYM",// Label
        LADSPA_PROPERTY_HARD_RT_CAPABLE,// Properties
        "YM2149",// Name
        "Andrzej Cichocki",// Maker
        "Andrzej Cichocki",// Copyright
        PortCount,//
        _PortDescriptors,//
        _PortNames,//
        _PortRangeHints,//
        0,// ImplementationData
        dizzYM::instantiate,//
        dizzYM::connect_port,//
        dizzYM::activate,//
        dizzYM::run,//
        0,// run_adding
        0,// set_run_adding_gain
        0,// deactivate
        dizzYM::cleanup,//
    };
    _dssiDescriptor = { //
        1,// API version, must be 1.
        &_ladspaDescriptor,//
        0,// configure()
        0,// get_program()
        0,// select_program()
        dizzYM::get_midi_controller_for_port,//
        dizzYM::run_synth,//
        0,// run_synth_adding()
        0,// run_multiple_synths()
        0,// run_multiple_synths_adding()
    };
}

Descriptor::~Descriptor() {
    delete[] _PortDescriptors;
    delete[] _PortNames;
    delete[] _PortRangeHints;
}

static Descriptor descriptor;

extern "C" {

const DSSI_Descriptor *dssi_descriptor(unsigned long index) {
    if (index == 0) {
        return &descriptor._dssiDescriptor;
    }
    else {
        return 0;
    }
}

}
