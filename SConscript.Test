Import('context')

env = context.newenv()
env.Append(CPPDEFINES = ['BOOST_TEST_DYN_LINK', 'CARSTAIRS_TEST', 'CARSTAIRS_LEVEL_INFO'])
env.Append(LIBS = ['boost_system', 'boost_unit_test_framework'])

env.Program('testcarstairs', context.sources())
