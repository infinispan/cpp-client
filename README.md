# cpp-client #

C++ client for Infinispan, over the [Hot Rod] (https://docs.jboss.org/author/display/ISPN/Hot+Rod+Protocol) wire protocol.
Released under Apache License, Version 2.0.

## Build ##

Build prerequisites: 

  CMake 2.8 (2.6 may work except for swig subset)
  C++03 plus shared_ptr TR1 support.
  Infinispan server distribution (located via ISPN_HOME or cmake directive)
  Java (any version compatible with the Infinispan distribution)
  Python 2.6 or newer
  
optional: SWIG

On Linux, use the package manager for your distribution to install the
necessary packages.  You may have to point to a special repository,
such as EPEL, to get CMake 2.8.

On Windows, Visual Studio 2008 + SP1, or any newer version, is
required.  You can obtain Windows installable distributions of other
needed software from:

  http://www.python.org/download/releases/
  http://www.swig.org/download.html
  http://www.cmake.org/cmake/resources/software.html
  

To build (where "srcdir" is the directory containg this README):

  os_make_directory my_build  [arbitrary location]
  cd my_build
  cmake [-DHOTROD_ISPN_HOME=/path/to/ispn/dist] path/to/srcdir
  make (Posix) | Visual Studio Build (Windows)
  ctest [-V]


The Java swig module is experimental.  See sandbox/mkjar.sh for
building the JNI libraries and uncomment the line:

  # add_subdirectory(test/swig)

in the top level CMakeLists.txt file.
