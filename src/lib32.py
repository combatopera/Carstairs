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

Import('context libs32')

import os
if 'DRONE' not in os.environ:

    env = context.newenv(libs32)
    env.Append(CXXFLAGS = ['-Winline', '-m32'])
    env.Append(LINKFLAGS = ['-m32'])

    env.SharedLibrary('carstairs', context.sources())
    env.Command('foreignsyms.txt', 'libcarstairs.so', '../pyven/foreignsyms $SOURCE | tee $TARGET')
