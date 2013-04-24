############ ENVIRONMENT DEFINITION

CPPPATH = ['include',
           '/usr/local/include',
           '/usr/local/Cellar/boost/1.51.0/include']

LIBPATH = ['/usr/local/lib',
           '/usr/local/Cellar/boost/1.51.0/lib']

LIBS = ['freeimageplus',
        'boost_program_options-mt']

env = Environment(LIBS=LIBS,
                  LIBPATH=LIBPATH,
                  CXX="/usr/bin/clang++",
                  CPPPATH=CPPPATH)


############ PROGRAM DEFINITION

source = [Glob('src/*')]

env.Program(target='seamcarve',
            source=source)
