#include "plugin.h"

#include <ladspa.h>
#include <stddef.h>
#include <array>

#include "dizzYM.h"

Descriptor::Descriptor() {
    size_t portCount = dizzYM::PORT_INFOS.size();
    _PortDescriptors = new LADSPA_PortDescriptor[portCount];
    _PortNames = new const char *[portCount];
    _PortRangeHints = new LADSPA_PortRangeHint[portCount];
    for (unsigned i = 0; i < portCount; ++i) {
        _PortDescriptors[i] = dizzYM::PORT_INFOS[i]._descriptor;
        _PortNames[i] = dizzYM::PORT_INFOS[i]._name;
        _PortRangeHints[i] = dizzYM::PORT_INFOS[i]._rangeHint; // Copy.
    }
    _ladspaDescriptor = { //
        0,// UniqueID
        "dizzYM",// Label
        LADSPA_PROPERTY_HARD_RT_CAPABLE,// Properties
        "YM2149",// Name
        "Andrzej Cichocki",// Maker
        "Andrzej Cichocki",// Copyright
        portCount,//
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
