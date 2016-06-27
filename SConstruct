import os, re, subprocess

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
            exports = {'context': self},
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
        env.Append(LIBS = ['fftw3', 'boost_filesystem'])
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

Context('Debug', src).enter()
Context('Test', src, test).enter()
