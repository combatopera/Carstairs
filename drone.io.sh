#!/bin/bash

# Copyright 2016 Andrzej Cichocki

# This file is part of Carstairs.
#
# Carstairs is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Carstairs is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

set -e

hg clone https://bitbucket.org/combatopera/pyven ../pyven

../pyven/drone.io.py

echo 2 | sudo update-alternatives --config gcc

sudo apt-get install dssi-dev libboost1.48-dev libboost-filesystem1.48.0 libboost-test1.48.0 python3-dev libfftw3-dev cppcheck zip

scons
