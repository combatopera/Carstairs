#include "plugin.h"

#include <alsa/seq_event.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <algorithm>
#include <cstring>
#include <string>

#include "../carstairs.h"
#include "../py/interpreter.h"
#include "../py/py.h"
#include "../util/util.h"

namespace {

Log const LOG(__FILE__);

Config const CONFIG; // Must be in same file as PortInfo for static init order.

PortInfoEnum const PortInfo {CONFIG}; // Must be in same file as descriptor for static init order.

Python const PYTHON;

Programs const PROGRAMS(CONFIG, PYTHON);

Descriptors const DESCRIPTORS(CONFIG, PortInfo.values());

LADSPA_Handle instantiate(LADSPA_Descriptor const *Descriptor, DSSI::cursor SampleRate) {
    CARSTAIRS_DEBUG("LADSPA: instantiate(%lu)", SampleRate);
    return new Carstairs(CONFIG, PortInfo, PYTHON, int(SampleRate));
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

DSSI_Program_Descriptor const *get_program(LADSPA_Handle Instance, DSSI::cursor Index) {
    CARSTAIRS_TRACE("DSSI: get_program(%lu)", Index);
    return PROGRAMS.programOrNull(sizex(Index));
}

int get_midi_controller_for_port(LADSPA_Handle, DSSI::cursor Port) {
    CARSTAIRS_DEBUG("DSSI: get_midi_controller_for_port(%lu)", Port);
    return PortInfo.values()[sizex(Port)]->_controllers;
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

Programs::Programs(Config const& config, Python const& python) {
    using namespace boost::filesystem;
    auto const suffix = ".py";
    auto const suffixLen = strlen(suffix);
    Interpreter(config, python).runTask([&] {
        for (auto i = directory_iterator(config._modulesDir), end = directory_iterator(); end != i; ++i) {
            auto const filename = i->path().filename().string();
            auto const suffixIndex = filename.find(suffix);
            if (filename.length() - suffixLen == suffixIndex) {
                std::string moduleName(filename);
                moduleName.erase(suffixIndex);
                auto const module = Interpreter::import(moduleName.c_str());
                if (module) {
                    auto const abstract = module.getAttr("_abstract");
                    if (abstract && abstract.boolValue()) {
                        CARSTAIRS_INFO("Ignoring abstract module: %s", moduleName.c_str());
                    }
                    else {
                        char * const programName = new char[moduleName.length() + 1];
                        strcpy(programName, moduleName.c_str());
                        _programs.push_back( {BANK, DSSI::cursor(-1), programName});
                    }
                }
                else {
                    CARSTAIRS_ERROR("Failed to load module: %s", moduleName.c_str());
                }
            }
        }
    });
    std::sort(_programs.begin(), _programs.end(), [](DSSI_Program_Descriptor const& a, DSSI_Program_Descriptor const& b) {
        return std::string(a.Name) < b.Name; // Not efficient, never mind.
        });
    for (auto i = sizex(_programs.size() - 1); SIZEX_NEG != i; --i) {
        _programs[i].Program = i;
    }
}

Programs::~Programs() {
    for (auto const program : _programs) {
        delete[] program.Name;
    }
}

Descriptors::Descriptors(Config const& config, Values<PortInfo_t const> const& ports)
        : _PortDescriptors(new LADSPA_PortDescriptor[ports.length]), //
        _PortNames(new char const *[ports.length]), //
        _PortRangeHints(new LADSPA_PortRangeHint[ports.length]) {
    for (auto i = ports.length - 1; SIZEX_NEG != i; --i) {
        _PortDescriptors[i] = ports[i]->_descriptor;
        _PortNames[i] = ports[i]->_name;
        _PortRangeHints[i] = ports[i]->_rangeHint; // Copy.
    }
    _ladspaDescriptor = { //
        config._UniqueID,// UniqueID i.e. a globally unique ID for this descriptor.
        "CARSTAIRS",// Label i.e. a key for this descriptor in the file.
        LADSPA_PROPERTY_HARD_RT_CAPABLE,// Properties
        "Carstairs",// Name i.e. the friendly name.
        "Andrzej Roman Cichocki",// Maker
        "Copyright (C) 2016 Andrzej Roman Cichocki",// Copyright
        ports.length,//
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
        get_program,//
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
    return Index ? 0 : DESCRIPTORS.dssiDescriptor();
}

}
