#include "plugin.h"

#include <alsa/seq_event.h>
#include <boost/iterator/iterator_facade.hpp>
#include <cstring>

#include "../carstairs.h"
#include "../py/interpreter.h"
#include "../py/py.h"
#include "../util/util.h"

namespace {

Log const LOG(__FILE__);

Config const CONFIG; // Must be in same file as PortInfo for static init order.

PortInfoEnum const PortInfo {CONFIG}; // Must be in same file as descriptor for static init order.

Module const MODULE;

Python const PYTHON;

Bounds<int> PROGRAM_INDEX_BOUNDS {0, 0xff};

ProgramInfos const PROGRAM_INFOS(CONFIG, MODULE, PYTHON);

Descriptors const DESCRIPTORS(CONFIG, PortInfo.values());

LADSPA_Handle instantiate(LADSPA_Descriptor const *Descriptor, DSSI::cursor SampleRate) {
    CARSTAIRS_DEBUG("LADSPA: instantiate(%lu)", SampleRate);
    return new Carstairs(CONFIG, PortInfo, MODULE, PYTHON, PROGRAM_INFOS, int(SampleRate));
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
    return PROGRAM_INFOS.descriptorOrNull(sizex(Index));
}

void select_program(LADSPA_Handle Instance, DSSI::cursor Bank, DSSI::cursor Program) {
    CARSTAIRS_DEBUG("DSSI: select_program(%lu, %lu)", Bank, Program);
    if (ProgramInfo::BANK == Bank && Program < PROGRAM_INFOS.size()) {
        static_cast<Carstairs *>(Instance)->selectProgram(sizex(Program));
    }
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

ProgramInfos::ProgramInfos(Config const& config, Module const& module, Python const& python) {
    auto const suffix = ".py";
    auto const suffixLen = strlen(suffix);
    using namespace boost::filesystem;
    CARSTAIRS_INFO("Scanning: %s", config._modulesDir.c_str());
    for (auto i = directory_iterator(config._modulesDir), end = directory_iterator(); end != i; ++i) {
        auto const& path = i->path();
        auto const filename = path.filename().string(); // XXX: Why doesn't a reference work here?
        auto const suffixIndex = filename.find(suffix);
        if (filename.size() - suffixLen == suffixIndex) {
            std::string moduleName(filename);
            moduleName.erase(suffixIndex);
            Interpreter(config, module, python).runTask([&] {
                addOrLog(path, moduleName);
            });
        }
    }
}

void ProgramInfos::addOrLog(boost::filesystem::path const& path, std::string const& moduleName) {
    auto const module = Interpreter::import(moduleName.c_str());
    if (module) {
        auto const program = module.getAttr("program");
        if (program) {
            auto const indexObj = program.getAttr("index");
            if (indexObj) {
                auto const indexOrNeg = indexObj.numberRoundToInt(); // TODO LATER: Don't round.
                if (PROGRAM_INDEX_BOUNDS.accept(indexOrNeg)) {
                    auto const index = sizex(indexOrNeg);
                    if (index >= _infos.size()) {
                        while (index != _infos.size()) {
                            _infos.push_back(std::unique_ptr<ProgramInfo>(new DefaultProgramInfo(index)));
                        }
                        _infos.push_back(std::unique_ptr<ProgramInfo>(new ProgramInfoImpl(index, path, moduleName)));
                    }
                    else if (!_infos[index].get()->isReal()) {
                        _infos[index] = std::unique_ptr<ProgramInfo>(new ProgramInfoImpl(index, path, moduleName));
                    }
                    else {
                        CARSTAIRS_ERROR("[%s] Program index %u already taken!", moduleName.c_str(), index);
                    }
                }
                else {
                    CARSTAIRS_ERROR("[%s] program.index out of bounds: %d", moduleName.c_str(), indexOrNeg);
                }
            }
            else {
                CARSTAIRS_INFO("[%s] No program.index, ignoring module.", moduleName.c_str()); // Probably abstract.
            }
        }
        else {
            CARSTAIRS_INFO("[%s] No program object, ignoring module.", moduleName.c_str()); // Could be a plain old module.
        }
    }
    else {
        CARSTAIRS_ERROR("[%s] Failed to load module.", moduleName.c_str());
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
        select_program,//
        get_midi_controller_for_port,//
        run_synth,//
        0,// run_synth_adding()
        0,// run_multiple_synths()
        0,// run_multiple_synths_adding()
    };
    CARSTAIRS_DEBUG("Constructed the Descriptors.");
}

Descriptors::~Descriptors() {
    CARSTAIRS_DEBUG("Destroying the Descriptors.");
    delete[] _PortDescriptors;
    delete[] _PortNames;
    delete[] _PortRangeHints;
}

extern "C" {

const DSSI_Descriptor *dssi_descriptor(DSSI::cursor Index) {
    return Index ? 0 : &DESCRIPTORS.dssiDescriptor();
}

}
