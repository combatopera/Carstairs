#include "program.h"

#include <boost/filesystem/operations.hpp>
#include <unistd.h>
#include <cassert>

#include "py/main.h"
#include "util/util.h"

namespace {

Log const LOG(__FILE__);

Python const PYTHON;

}

ProgramImpl::ProgramImpl(char const *name)
        : Interpreter(PYTHON) {
    info("Reloading module: %s", name);
    runTask([&] {
        _module = import(name);
        if (_module) {
            _rate = _module.getAttr("rate").numberToFloatOr(50);
            debug("Program rate: %.3f", _rate);
        }
        else {
            error("Failed to reload module, program will not change.");
        }
    });
}

ProgramImpl::~ProgramImpl() {
    if (_module) {
        debug("Disposing module.");
        runTask([&] {
            _module=0;
        });
    }
}

Loader::Loader(Config const& config)
        : _moduleName(config._programModule), _mark(-1) {
    info("Loading module: %s", _moduleName);
    Interpreter(PYTHON).runTask([&] {
        auto const module = Interpreter::import(_moduleName);
        if (module) {
            auto const bytes = module.getAttr("__file__").toPathBytes();
            auto const str = bytes.unwrapBytes();
            info("Module path: %s", str);
            _path = str;
            _thread = std::thread([&] {
                        poll();
                    });
        }
        else {
            error("Failed to load module, refresh disabled.");
        }
    });
}

Loader::~Loader() {
    if (_thread.joinable()) {
        debug("Notifying loader thread.");
        _path.clear();
        _thread.join();
        debug("Loader thread has terminated.");
    }
}

void Loader::poll() {
    debug("Loader thread running.");
    while (!_path.empty()) {
        auto const mark = boost::filesystem::last_write_time(_path);
        if (mark != _mark) {
            _mark = mark;
            std::shared_ptr<ProgramImpl> programHolder(new ProgramImpl(_moduleName));
            ProgramImpl const& program = *programHolder.get();
            if (program) {
                _nextProgram = programHolder;
                debug("New program ready.");
            }
        }
        sleep(1);
    }
}

void ProgramImpl::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    assert(_module);
    runTask([&] {
        if (offFrameOrNeg < 0) {
            _module.getAttr("on").callVoid("(i)", noteFrame);
        }
        else {
            _module.getAttr("off").callVoid("ii", noteFrame, offFrameOrNeg);
        }
        auto const chan = _module.getAttr("chan");
        if (chan) {
            auto const level = chan.getAttr("level");
            if (level) {
                state.setLevel4(level.numberRoundToInt());
            }
        }
    });
}
