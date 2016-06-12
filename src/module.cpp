#include "module.h"

#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>
#include <fstream>
#include <string>

#include "util/util.h"

namespace {

Log const LOG(__FILE__);

char const * const CODE =
        R"EOF(class Poly:

    def __init__(self, script):
        import re
        self.v = []
        for word in re.findall(r'[\S]+', script):
            sep = word.find('>')
            if -1 == sep:
                sep = word.find('x')
                if -1 == sep:
                    self.v.append(float(word))
                else:
                    n = int(word[sep+1:])
                    self.v += [float(word[:sep])] * n
            else:
                n = int(word[:sep])
                src = self.v[-1]
                diff = float(word[sep+1:]) - src
                for i in range(n):
                    self.v.append(src + diff * (1+i) / n)

    def __getitem__(self, i):
        n = len(self.v)
        return self.v[i] if i < n else self.v[n - 1]

class Chip:

    clock = %2%

chip = Chip()
del Chip

class Channel:

    pass

A = Channel()
del Channel

class Note:

    def envperiod(self):
        return chip.clock / (%1% * chip.envshape.wavelength * self.freq)

note = Note()
del Note

class Program:

    rate = 50

program = Program()
del Program
)EOF";

}

Module::Module(Config const& config)
        : _dir(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path()) {
    boost::filesystem::create_directory(_dir);
    std::ofstream f;
    f.open((_dir / "carstairs.py").string().c_str());
    f << boost::format(CODE) % config.YM2149_ATOM_SIZE % config._nominalClock;
    f.close();
}

Module::~Module() {
    CARSTAIRS_DEBUG("Deleting: %s", _dir.c_str());
    boost::filesystem::remove_all(_dir);
}
