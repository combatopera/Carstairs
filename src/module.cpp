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

class EnvShape:

    def __init__(self, att_alt_hold):
        alt_hold = att_alt_hold & 0b11
        if 0 == alt_hold: # Saw.
            self.wavelength = 32
        elif 2 == alt_hold: # Triangle.
            self.wavelength = 64
        self.index = 0b1000 | att_alt_hold

desc_saw = EnvShape(0)
desc_hold = EnvShape(1)
desc_tri = EnvShape(2)
desc_flip = EnvShape(3)
asc_saw = EnvShape(4)
asc_hold = EnvShape(5)
asc_tri = EnvShape(6)
asc_flip = EnvShape(7)
del EnvShape

class Chip:

    clock = %2%

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)
        if 'envshape' == name:
            object.__setattr__(self, 'envshapechanged', True)

    def envfreq(self):
        return self.clock / (%1% * self.envshape.wavelength * self.envperiod)

chip = Chip()
del Chip

class Channel:

    pass

A = Channel()
del Channel

class Note:

    def toneperiod(self, freq = None):
        return chip.clock / (%1% * 2 * (self.freq if freq is None else freq))

    def envperiod(self):
        return chip.clock / (%1% * chip.envshape.wavelength * self.freq)

note = Note()
del Note

class Program:

    rate = 50 # Default if the program doesn't set one.

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
