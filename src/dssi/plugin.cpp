#include "plugin.h"

#include <alsa/seq_event.h>
#include <ladspa.h>

#include "../dizzYM.h"
#include "../util/util.h"
#include "port.h"

static LADSPA_Handle instantiate(const LADSPA_Descriptor *Descriptor, cursor_t SampleRate) {
    debug("LADSPA: instantiate");
    return new dizzYM((int) SampleRate);
}

static void activate(LADSPA_Handle Instance) {
    debug("LADSPA: activate");
    ((dizzYM *) Instance)->reset();
}

static void deactivate(LADSPA_Handle Instance) {
    debug("LADSPA: deactivate");
}

static void connect_port(LADSPA_Handle Instance, cursor_t Port, LADSPA_Data *DataLocation) {
    debug("LADSPA: connect_port");
    ((dizzYM *) Instance)->setPortValPtr((int) Port, DataLocation);
}

static int get_midi_controller_for_port(LADSPA_Handle, cursor_t Port) {
    debug("DSSI: get_midi_controller_for_port");
    return PortInfo._values.at(Port)->_controllers;
}

static void run(LADSPA_Handle Instance, cursor_t SampleCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, 0, 0);
}

static void run_synth(LADSPA_Handle Instance, cursor_t SampleCount, snd_seq_event_t *Events, cursor_t EventCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, Events, EventCount);
}

static void cleanup(LADSPA_Handle Instance) {
    debug("LADSPA: cleanup");
    delete (dizzYM *) Instance;
    debug("Cleaned up.");
}

Descriptor::Descriptor() {
    _PortDescriptors = new LADSPA_PortDescriptor[PortInfo._values._n];
    _PortNames = new const char *[PortInfo._values._n];
    _PortRangeHints = new LADSPA_PortRangeHint[PortInfo._values._n];
    for (index_t i = 0; i < PortInfo._values._n; ++i) {
        _PortDescriptors[i] = PortInfo._values.at(i)->_descriptor;
        _PortNames[i] = PortInfo._values.at(i)->_name;
        _PortRangeHints[i] = PortInfo._values.at(i)->_rangeHint; // Copy.
    }
    _ladspaDescriptor = { //
        0,// UniqueID
        "dizzYM",// Label
        LADSPA_PROPERTY_HARD_RT_CAPABLE,// Properties
        "YM2149",// Name
        "Andrzej Cichocki",// Maker
        "Andrzej Cichocki",// Copyright
        PortInfo._values._n,//
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
        deactivate,//
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

PortInfoEnum PortInfo; // Must be in same file as descriptor for static init order.

static Descriptor descriptor;

extern "C" {

const DSSI_Descriptor *dssi_descriptor(cursor_t Index) {
    if (Index == 0) {
        return &descriptor._dssiDescriptor;
    }
    else {
        return 0;
    }
}

}
