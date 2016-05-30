#include "plugin.h"

#include <alsa/seq_event.h>

#include "../config.h"
#include "../dizzYM.h"
#include "../util/buf.h"
#include "../util/util.h"
#include "port.h"

static Log const LOG(__FILE__);

static Config const CONFIG; // Must be in same file as PortInfo for static init order.

static PortInfoEnum const PortInfo {CONFIG}; // Must be in same file as descriptor for static init order.

static LADSPA_Handle instantiate(const LADSPA_Descriptor *Descriptor, DSSI::cursor SampleRate) {
    debug("LADSPA: instantiate");
    return new dizzYM(CONFIG, PortInfo, int(SampleRate));
}

static void activate(LADSPA_Handle Instance) {
    debug("LADSPA: activate");
    static_cast<dizzYM *>(Instance)->start();
}

static void deactivate(LADSPA_Handle Instance) {
    debug("LADSPA: deactivate");
}

static void connect_port(LADSPA_Handle Instance, DSSI::cursor Port, LADSPA_Data *DataLocation) {
    debug("LADSPA: connect_port");
    static_cast<dizzYM *>(Instance)->setPortValPtr((int) Port, DataLocation);
}

static int get_midi_controller_for_port(LADSPA_Handle, DSSI::cursor Port) {
    debug("DSSI: get_midi_controller_for_port");
    return PortInfo._values.at(sizex(Port))->_controllers;
}

static void run(LADSPA_Handle Instance, DSSI::cursor SampleCount) {
    static_cast<dizzYM *>(Instance)->runSynth(SampleCount, 0, 0);
}

static void run_synth(LADSPA_Handle Instance, DSSI::cursor SampleCount, snd_seq_event_t *Events, DSSI::cursor EventCount) {
    static_cast<dizzYM *>(Instance)->runSynth(SampleCount, Events, EventCount);
}

static void cleanup(LADSPA_Handle Instance) {
    debug("LADSPA: cleanup");
    delete static_cast<dizzYM *>(Instance);
    debug("Cleaned up.");
}

Descriptors::Descriptors() {
    _PortDescriptors = new LADSPA_PortDescriptor[PortInfo._values._n];
    _PortNames = new char const *[PortInfo._values._n];
    _PortRangeHints = new LADSPA_PortRangeHint[PortInfo._values._n];
    for (auto i = PortInfo._values._n - 1; SIZEX_NEG != i; --i) {
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
    debug("Constructed the Descriptors.");
}

DSSI_Descriptor const *Descriptors::dssiDescriptor() const {
    return &_dssiDescriptor;
}

Descriptors::~Descriptors() {
    debug("Destroying the Descriptors.");
    delete[] _PortDescriptors;
    delete[] _PortNames;
    delete[] _PortRangeHints;
}

static Descriptors const descriptors;

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
