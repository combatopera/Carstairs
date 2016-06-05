#include "program.h"

#include <unistd.h>
#include <cassert>

#include "py/py.h"
#include "util/util.h"

namespace {
Log const LOG(__FILE__);
}

ProgramImpl::ProgramImpl(Config const& config, Python const& python, ProgramInfo const& info)
        : Interpreter(config, python), _info(info) {
    auto const name = info.descriptor().Name;
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

Loader::Loader(Config const& config, Python const& python, ProgramInfos const& programInfos)
        : _python(python), _programs(new std::shared_ptr<Program const>[programInfos.size()]), _programInfos(programInfos) {
    _flag = true;
    _thread = std::thread([&] {
        poll(config);
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
}

void Loader::poll(Config const& config) {
    CARSTAIRS_DEBUG("Loader thread running.");
    while (_flag) {
        for (auto i = _programInfos.size() - 1; SIZEX_NEG != i; --i) {
            auto& info = _programInfos[i];
            if (info.reload()) {
                std::shared_ptr<ProgramImpl> programHolder(new ProgramImpl(config, _python, info));
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

void ProgramImpl::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    assert(_rate);
    runTask([&] {
        auto const module = import(_info.descriptor().Name);
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
