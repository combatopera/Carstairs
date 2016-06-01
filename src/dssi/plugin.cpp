#include "plugin.h"

#include <alsa/seq_event.h>

#include "../carstairs.h"
#include "../util/buf.h"
#include "../util/util.h"
#include "port.h"

namespace {

Log const LOG(__FILE__);

Config const CONFIG; // Must be in same file as PortInfo for static init order.

PortInfoEnum const PortInfo {CONFIG}; // Must be in same file as descriptor for static init order.

Descriptors const descriptors {CONFIG};

LADSPA_Handle instantiate(const LADSPA_Descriptor *Descriptor, DSSI::cursor SampleRate) {
    CARSTAIRS_DEBUG("LADSPA: instantiate(%lu)", SampleRate);
    return new Carstairs(CONFIG, PortInfo, int(SampleRate));
}

void activate(LADSPA_Handle Instance) {
    CARSTAIRS_DEBUG("LADSPA: activate");
    static_cast<Carstairs *>(Instance)->start();
}

void deactivate(LADSPA_Handle Instance) {
    CARSTAIRS_DEBUG("LADSPA: deactivate");
}

void connect_port(LADSPA_Handle Instance, DSSI::cursor Port, LADSPA_Data *DataLocation) {
    CARSTAIRS_DEBUG("LADSPA: connect_port(%lu)", Port);
    static_cast<Carstairs *>(Instance)->setPortValPtr((int) Port, DataLocation);
}

int get_midi_controller_for_port(LADSPA_Handle, DSSI::cursor Port) {
    CARSTAIRS_DEBUG("DSSI: get_midi_controller_for_port(%lu)", Port);
    return PortInfo._values.at(sizex(Port))->_controllers;
}

void run(LADSPA_Handle Instance, DSSI::cursor SampleCount) {
    static_cast<Carstairs *>(Instance)->runSynth(SampleCount, 0, 0);
}

void run_synth(LADSPA_Handle Instance, DSSI::cursor SampleCount, snd_seq_event_t *Events, DSSI::cursor EventCount) {
    static_cast<Carstairs *>(Instance)->runSynth(SampleCount, Events, EventCount);
}

void cleanup(LADSPA_Handle Instance) {
    CARSTAIRS_DEBUG("LADSPA: cleanup");
    delete static_cast<Carstairs *>(Instance);
    CARSTAIRS_DEBUG("Cleaned up.");
}

}

Descriptors::Descriptors(Config const& config) {
    _PortDescriptors = new LADSPA_PortDescriptor[PortInfo._values._n];
    _PortNames = new char const *[PortInfo._values._n];
    _PortRangeHints = new LADSPA_PortRangeHint[PortInfo._values._n];
    for (auto i = PortInfo._values._n - 1; SIZEX_NEG != i; --i) {
        _PortDescriptors[i] = PortInfo._values.at(i)->_descriptor;
        _PortNames[i] = PortInfo._values.at(i)->_name;
        _PortRangeHints[i] = PortInfo._values.at(i)->_rangeHint; // Copy.
    }
    _ladspaDescriptor = { //
        config._UniqueID,// UniqueID i.e. a globally unique ID for this descriptor.
        "CARSTAIRS",// Label i.e. a key for this descriptor in the file.
        LADSPA_PROPERTY_HARD_RT_CAPABLE,// Properties
        "Carstairs",// Name i.e. the friendly name.
        "Andrzej Roman Cichocki",// Maker
        "Copyright (C) 2016 Andrzej Roman Cichocki",// Copyright
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
    CARSTAIRS_DEBUG("Constructed the Descriptors.");
}

DSSI_Descriptor const *Descriptors::dssiDescriptor() const {
    return &_dssiDescriptor;
}

Descriptors::~Descriptors() {
    CARSTAIRS_DEBUG("Destroying the Descriptors.");
    delete[] _PortDescriptors;
    delete[] _PortNames;
    delete[] _PortRangeHints;
}

extern "C" {

const DSSI_Descriptor *dssi_descriptor(DSSI::cursor Index) {
    if (Index == 0) {
        return descriptors.dssiDescriptor();
    }
    else {
        return 0;
    }
}

}
