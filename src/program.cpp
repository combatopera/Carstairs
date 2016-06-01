#include "program.h"

#include <boost/filesystem/operations.hpp>
#include <unistd.h>
#include <cassert>

#include "py/py.h"
#include "util/util.h"

namespace {
Log const LOG(__FILE__);
}

ProgramImpl::ProgramImpl(Config const& config, Python const& python, char const *name)
        : Interpreter(config, python), _name(name) {
    CARSTAIRS_INFO("Reloading module: %s", name);
    runTask([&] {
        auto const module = import(name);
        if (module) {
            _rate = module.getAttr("rate").numberToFloatOr(50);
            CARSTAIRS_DEBUG("Program rate: %.3f", _rate);
        }
        else {
            CARSTAIRS_ERROR("Failed to reload module, program will not change.");
        }
    });
}

Loader::Loader(Config const& config, Python const& python)
        : _python(python), _moduleName(config._programModule), _mark(-1) {
    CARSTAIRS_INFO("Loading module: %s", _moduleName);
    Interpreter(config, python).runTask([&] {
        auto const module = Interpreter::import(_moduleName);
        if (module) {
            auto const bytes = module.getAttr("__file__").toPathBytes();
            auto const str = bytes.unwrapBytes();
            CARSTAIRS_INFO("Module path: %s", str);
            _path = str;
            _flag = true;
            _thread = std::thread([&] {
                        poll(config);
                    });
        }
        else {
            CARSTAIRS_ERROR("Failed to load module, refresh disabled.");
        }
    });
}

Loader::~Loader() {
    if (_thread.joinable()) {
        CARSTAIRS_DEBUG("Notifying loader thread.");
        _flag = false;
        _thread.join();
        CARSTAIRS_DEBUG("Loader thread has terminated.");
    }
}

void Loader::poll(Config const& config) {
    CARSTAIRS_DEBUG("Loader thread running.");
    while (_flag) {
        auto const mark = boost::filesystem::last_write_time(_path);
        if (mark != _mark) {
            _mark = mark;
            std::shared_ptr<ProgramImpl> programHolder(new ProgramImpl(config, _python, _moduleName));
            ProgramImpl const& program = *programHolder.get();
            if (program) {
                _nextProgram = programHolder;
                CARSTAIRS_DEBUG("New program ready.");
            }
        }
        sleep(1);
    }
}

void ProgramImpl::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    assert(_rate);
    runTask([&] {
        auto const module = import(_name);
        if (offFrameOrNeg < 0) {
            module.getAttr("on").callVoid("(i)", noteFrame);
        }
        else {
            module.getAttr("off").callVoid("ii", noteFrame, offFrameOrNeg);
        }
        auto const chan = module.getAttr("chan");
        if (chan) {
            auto const level = chan.getAttr("level");
            if (level) {
                state.setLevel4(level.numberRoundToInt());
            }
        }
    });
}
