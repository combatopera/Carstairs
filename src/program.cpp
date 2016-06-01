#include "program.h"

#include <boost/filesystem/operations.hpp>
#include <unistd.h>
#include <cassert>

#include "py/main.h"
#include "py/py.h"
#include "util/util.h"

namespace {

Log const LOG(__FILE__);

Python const PYTHON;

}

ProgramImpl::ProgramImpl(char const *name)
        : Interpreter(PYTHON), _name(name) {
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

Loader::Loader(Config const& config)
        : _moduleName(config._programModule), _mark(-1) {
    CARSTAIRS_INFO("Loading module: %s", _moduleName);
    Interpreter(PYTHON).runTask([&] {
        auto const module = Interpreter::import(_moduleName);
        if (module) {
            auto const bytes = module.getAttr("__file__").toPathBytes();
            auto const str = bytes.unwrapBytes();
            CARSTAIRS_INFO("Module path: %s", str);
            _path = str;
            _thread = std::thread([&] {
                        poll();
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
        _path.clear();
        _thread.join();
        CARSTAIRS_DEBUG("Loader thread has terminated.");
    }
}

void Loader::poll() {
    CARSTAIRS_DEBUG("Loader thread running.");
    while (!_path.empty()) {
        auto const mark = boost::filesystem::last_write_time(_path);
        if (mark != _mark) {
            _mark = mark;
            std::shared_ptr<ProgramImpl> programHolder(new ProgramImpl(_moduleName));
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
