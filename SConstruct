import os, re, subprocess, versions

class Libs:

    def __init__(self):
        self.paths = {}
        namepattern = re.compile('^lib(.+)[.]so[.](.+)$')
        for path in re.compile(' => (.+)$', re.MULTILINE).findall(subprocess.check_output(['/sbin/ldconfig', '-p'])):
            m = namepattern.search(os.path.basename(path))
            if m is not None:
                self.paths[m.groups()] = path

    def __getitem__(self, (lib, version)):
        return self.paths[lib, version]

class Tree:

    def __init__(self, path, *types):
        self.pattern = re.compile('[.](?:%s)$' % '|'.join(re.escape(t) for t in types))
        self.path = path

    def __iter__(self):
        vardir = Dir('.').abspath
        searchdir = os.path.join(os.path.dirname(vardir), self.path)
        for dirpath, dirnames, filenames in os.walk(searchdir):
            for name in filenames:
                if self.pattern.search(name) is not None:
                    relpath = '.' + dirpath[len(searchdir):]
                    yield os.path.join(vardir, self.path, relpath, name)

class Context:

    def __init__(self, name, *trees):
        self.name = name
        self.trees = trees

    def enter(self):
        SConscript(
            self.name + '.py',
            variant_dir = self.name,
            duplicate = 0,
            exports = {'context': self, 'libs': libs, 'versions': versions},
        )

    def sources(self):
        def g():
            for tree in self.trees:
                for path in tree:
                    yield path
        return list(g())

    @staticmethod
    def newenv():
        env = Environment()
        env.Append(CXXFLAGS = [
            '-std=c++11',
            '-Og',
            '-g3',
            '-Wextra',
            '-Wall',
            '-Wconversion',
            '-fmessage-length=0',
        ])
        env.Append(LIBS = ['fftw3'])
        env.Append(LIBS = File(libs['boost_filesystem', versions.boost]))
        for word in re.findall(r'[\S]+', subprocess.check_output(['python3-config', '--ldflags'])):
            if word.startswith('-L'):
                env.Append(LIBPATH = word[2:])
            elif word.startswith('-l'):
                env.Append(LIBS = word[2:])
        for word in re.findall(r'[\S]+', subprocess.check_output(['python3-config', '--includes'])):
            if word.startswith('-I'):
                env.Append(CPPPATH = [word[2:]])
        return env

src = Tree('src', 'cpp')
test = Tree('test', 'cxx')

libs = Libs()

Context('Debug', src).enter()
Context('Test', src, test).enter()
