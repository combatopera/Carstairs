#include "program.h"

#include <boost/filesystem/operations.hpp>
#include <unistd.h>

#include "py/main.h"
#include "util/util.h"

namespace {

Log const LOG(__FILE__);

Python const PYTHON;

}

Program::Program(Config const& config)
        : _moduleName(config._programModule), _interpreter(PYTHON), _mark(-1) {
    debug("Loading module: %s", _moduleName);
    _interpreter.runTask([&] {
        auto const module = _interpreter.import(_moduleName);
        if (module) {
            auto const bytes = module.getAttr("__file__").toPathBytes();
            auto const str = bytes.unwrapBytes();
            debug("Module path: %s", str);
            _path = str;
            _pollEnabled = true;
            _thread = std::thread([&] {
                        debug("Background thread running.");
                        while (_pollEnabled) {
                            sleep(1);
                        }
                    });
        }
        else {
            debug("Failed to load module, refresh disabled.");
        }
    });
}

Program::~Program() {
    if (_thread.joinable()) {
        debug("Notifying background thread.");
        _pollEnabled = false;
        _thread.join();
        debug("Background thread terminated.");
    }
}

void Program::refresh() {
    if (!_path.empty()) {
        auto const mark = boost::filesystem::last_write_time(_path);
        if (mark != _mark) {
            _mark = mark;
            _interpreter = PYTHON;
            debug("Reloading module: %s", _moduleName);
            _interpreter.runTask([&] {
                _module = _interpreter.import(_moduleName);
                if (_module) {
                    _rate = _module.getAttr("rate").numberToFloatOr(DEFAULT_RATE);
                    debug("Program rate: %.3f", _rate);
                }
                else {
                    debug("Failed to reload module.");
                }
            });
        }
    }
}

void Program::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    if (!noteFrame) {
        state.setLevel4(13); // Use half the available amp.
    }
    if (_module) {
        _interpreter.runTask([&] {
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
}
