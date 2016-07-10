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

import os, sys

userhome = os.path.expanduser('~')

def load():
    try:
        dirs = os.environ['DSSI_PATH'].split(os.pathsep)
    except KeyError:
        dirs = [os.path.join(userhome, '.dssi')] # Skip system paths for now.
    for dir in dirs:
        print('Scanning:', dir, file = sys.stderr)
        path = os.path.join(dir, 'libcarstairs.py')
        if os.path.exists(path):
            print('Reading config:', path, file = sys.stderr)
            with open(path) as f:
                return f.read()

exec(load())
