#include "plugin.h"

#include <alsa/seq_event.h>
#include <ladspa.h>

#include "../dizzYM.h"
#include "../util.h"
#include "port.h"

static LADSPA_Handle instantiate(const LADSPA_Descriptor *Descriptor, unsigned long SampleRate) {
    debug("Instantiating.");
    return new dizzYM((int) SampleRate);
}

static void activate(LADSPA_Handle Instance) {
    debug("Activating.");
    ((dizzYM *) Instance)->reset();
}

static void connect_port(LADSPA_Handle Instance, unsigned long Port, LADSPA_Data *DataLocation) {
    ((dizzYM *) Instance)->setPortValPtr((int) Port, DataLocation);
}

static int get_midi_controller_for_port(LADSPA_Handle, unsigned long Port) {
    return CONSTANTS.PORT_INFOS[Port]->_controllers;
}

static void run(LADSPA_Handle Instance, unsigned long SampleCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, 0, 0);
}

static void run_synth(LADSPA_Handle Instance, unsigned long SampleCount, snd_seq_event_t *Events, unsigned long EventCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, Events, EventCount);
}

static void cleanup(LADSPA_Handle Instance) {
    delete (dizzYM *) Instance;
    debug("Cleaned up.");
}

Descriptor::Descriptor() {
    _PortDescriptors = new LADSPA_PortDescriptor[CONSTANTS.PortCount];
    _PortNames = new const char *[CONSTANTS.PortCount];
    _PortRangeHints = new LADSPA_PortRangeHint[CONSTANTS.PortCount];
    for (unsigned i = 0; i < CONSTANTS.PortCount; ++i) {
        _PortDescriptors[i] = CONSTANTS.PORT_INFOS[i]->_descriptor;
        _PortNames[i] = CONSTANTS.PORT_INFOS[i]->_name;
        _PortRangeHints[i] = CONSTANTS.PORT_INFOS[i]->_rangeHint; // Copy.
    }
    _ladspaDescriptor = { //
        0,// UniqueID
        "dizzYM",// Label
        LADSPA_PROPERTY_HARD_RT_CAPABLE,// Properties
        "YM2149",// Name
        "Andrzej Cichocki",// Maker
        "Andrzej Cichocki",// Copyright
        CONSTANTS.PortCount,//
        _PortDescriptors,//
        _PortNames,//
        _PortRangeHints,//
        0,// ImplementationData
        instantiate,//
        connect_port,//
        activate,//
        run,//
        0,// run_adding
        0,// set_run_adding_gain
        0,// deactivate
        cleanup,//
    };
    _dssiDescriptor = { //
        1,// API version, must be 1.
        &_ladspaDescriptor,//
        0,// configure()
        0,// get_program()
        0,// select_program()
        get_midi_controller_for_port,//
        run_synth,//
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

Constants CONSTANTS;

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
