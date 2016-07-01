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

Import('context libs versions')

env = context.newenv()
env.Append(CXXFLAGS = ['-Wno-unused-parameter'])
env.Append(CPPDEFINES = ['BOOST_TEST_DYN_LINK', 'CARSTAIRS_TEST', 'CARSTAIRS_LEVEL_INFO'])
env.Append(LIBS = [File(libs['boost_system', versions.boost]), File(libs['boost_unit_test_framework', versions.boost])])
env.Append(CPPPATH = 'src/main')

env.Program('testcarstairs', context.sources())
