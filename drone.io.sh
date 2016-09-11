#!/bin/bash

set -e

hg clone https://bitbucket.org/combatopera/pyven ../pyven

../pyven/drone.io.py

echo 2 | sudo update-alternatives --config gcc

sudo apt-get install dssi-dev libboost1.48-dev libboost-filesystem1.48.0 libboost-test1.48.0 python3-dev libfftw3-dev cppcheck zip

scons
