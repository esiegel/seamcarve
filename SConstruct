############ ENVIRONMENT DEFINITION

CPPPATH = ['include',
           '/usr/local/include',
           '/usr/local/Cellar/boost/1.53.0/include',
           '/Applications/qt5/5.0.2/clang_64/include/',
           '/Applications/qt5/5.0.2/clang_64/include/QtWidgets']

LIBPATH = ['/usr/local/lib',
           '/usr/local/Cellar/boost/1.53.0/lib',
           '/Applications/qt5/5.0.2/clang_64/libs']

LIBS = ['freeimageplus',
        'boost_program_options-mt']

# When using c++11 (shared_ptr, etc), boost must be compiled with clang support.
# Currently, c++11 support is commented out until boost support exists.
env = Environment(LIBS=LIBS,
                  LIBPATH=LIBPATH,
                  CXX="/usr/bin/clang++",
                  #CXX="/usr/bin/g++",
                  #CXXFLAGS="-std=c++11 -stdlib=libc++",
                  CPPPATH=CPPPATH)

############ SConscript paths

env.SConscript(['SConscript'],
               {'env': env},
               variant_dir='build')
