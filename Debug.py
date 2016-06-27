Import('context')

env = context.newenv()
env.Append(CXXFLAGS = ['-Wunused', '-Winline'])

env.SharedLibrary('carstairs', context.sources())
