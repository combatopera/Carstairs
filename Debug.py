Import('context')

env = context.newenv()
env.Append(CXXFLAGS = '-Winline')

env.SharedLibrary('carstairs', context.sources())
