############ ENVIRONMENT DEFINITION

CPPPATH = ['include',
           '/usr/local/include',
           '/usr/local/Cellar/boost/1.51.0/include']

LIBPATH = ['/usr/local/lib',
           '/usr/local/Cellar/boost/1.51.0/lib']

LIBS = ['freeimageplus',
        'boost_program_options-mt']

# When using c++11 (shared_ptr, etc), boost must be compiled with clang support.
# Currently, c++11 support is commented out until boost support exists.
env = Environment(LIBS=LIBS,
                  LIBPATH=LIBPATH,
                  CXX="/usr/bin/clang++",
                  #CXXFLAGS="-std=c++11 -stdlib=libc++",
                  CPPPATH=CPPPATH)

############ SConscript paths

env.SConscript(['SConscript'],
               {'env': env},
               variant_dir='build')
