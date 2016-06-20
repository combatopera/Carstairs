// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

#include "program.h"

#include <boost/filesystem/path.hpp>
#include <unistd.h>
#include <cassert>

#include "py/py.h"
#include "util/util.h"

namespace {
Log const LOG(__FILE__);
}

DefaultProgram::~DefaultProgram() {
    CARSTAIRS_DEBUG("Deleting a DefaultProgram.");
}

ProgramImpl::ProgramImpl(Config const& config, Module const& module, Python const& python, ProgramInfo const& info)
        : Interpreter(config._modulesDir, module.dir(), python), Program(config), _info(info) {
    auto const name = info.descriptor().Name;
    CARSTAIRS_INFO("Reloading module: %s", name);
    runTask([&] {
        auto const module = import(name);
        if (module) {
            _rate = module.getAttr("program").getAttr("rate").numberToFloat();
            CARSTAIRS_DEBUG("Program rate: %.3f", _rate);
        }
        else {
            CARSTAIRS_ERROR("Failed to reload module, program will not change.");
        }
    });
}

Loader::Loader(Config const& config, Module const& module, Python const& python, ProgramInfos const& programInfos)
        : _python(python), _defaultProgram(new DefaultProgram(config)), //
        _programs(new std::shared_ptr<Program const>[programInfos.size()]), //
        _marks(new std::time_t[programInfos.size()]), //
        _programInfos(programInfos) {
    for (sizex i = programInfos.size() - 1; SIZEX_NEG != i; --i) {
        _programs[i] = _defaultProgram; // FIXME LATER: Do an initial synchronous load instead.
        _marks[i] = -1;
    }
    _flag = true;
    _thread = std::thread([&] {
        poll(config, module);
    });
}

Loader::~Loader() {
    if (_thread.joinable()) {
        CARSTAIRS_DEBUG("Notifying loader thread.");
        _flag = false;
        _thread.join();
        CARSTAIRS_DEBUG("Loader thread has terminated.");
    }
    delete[] _programs;
    delete[] _marks;
}

void Loader::poll(Config const& config, Module const& module) {
    CARSTAIRS_DEBUG("Loader thread running.");
    while (_flag) {
        for (auto i = _programInfos.size() - 1; SIZEX_NEG != i; --i) {
            if (reload(i)) {
                std::shared_ptr<ProgramImpl> programHolder(new ProgramImpl(config, module, _python, _programInfos[i]));
                ProgramImpl const& program = *programHolder.get();
                if (program) {
                    _programs[i] = programHolder;
                    CARSTAIRS_DEBUG("New program ready.");
                }
            }
        }
        sleep(1);
    }
}

void ProgramImpl::fire(int noteFrame, int offFrameOrNeg, State& state, EnvShape& envShape) const {
    assert(_rate);
    if (!noteFrame) {
        state.setToneFlag(false);
        state.setNoiseFlag(false);
        state.setLevelMode(false);
    }
    runTask([&] {
        auto const module = import(_info.descriptor().Name);
        auto const note = module.getAttr("note"), chip = module.getAttr("chip");
        if (!noteFrame) {
            note.setAttr("freq", freq(state.midiNote()));
            chip.setAttr("envshape", module.getAttr(envShape.envShapeName()));
        }
        chip.setAttr("envshapechanged", 0L);
        note.setAttr("onframe", long(noteFrame));
        if (offFrameOrNeg < 0) {
            module.getAttr("on").callVoid("()");
        }
        else {
            note.setAttr("offframe", long(offFrameOrNeg));
            auto const off = module.getAttr("off");
            if (off) {
                off.callVoid("()");
            }
            else {
                module.getAttr("on").callVoid("()");
            }
        }
        auto chan = module.getAttr("A");
        if (chan) {
            auto var = chan.getAttr("level");
            if (var) {
                state.setLevel4(var.numberRoundToInt());
            }
            var = chan.getAttr("toneflag");
            if (var) {
                state.setToneFlag(var.boolValue());
            }
            var = chan.getAttr("noiseflag");
            if (var) {
                state.setNoiseFlag(var.boolValue());
            }
            var = chan.getAttr("levelmode");
            if (var) {
                state.setLevelMode(var.boolValue());
            }
            var = chan.getAttr("toneperiod");
            if (var) {
                state.setTP(var.numberRoundToInt());
            }
        }
        auto var = chip.getAttr("noiseperiod");
        if (var) {
            state.setNP(var.numberRoundToInt());
        }
        if (chip.getAttr("envshapechanged").boolValue()) {
            envShape.changeShape(chip.getAttr("envshape").getAttr("index").numberRoundToInt());
        }
        var = chip.getAttr("envperiod");
        if (var) {
            state.setEP(var.numberRoundToInt());
        }
    });
}
