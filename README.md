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

## Test with SWIG ##
The Java swig module is experimental and tested on Linux only.
Components needed to use swig tests: Apache Ant 1.8.x (http://ant.apache.org/), Apache Ivy 2.3.x (http://ant.apache.org/ivy/) and SWIG 2.0.x (http://www.swig.org/)
For building the JNI libraries and uncomment, in the top level CMakeLists.txt file, the line:

  # add_subdirectory(test/swig)
  build the project as described upward.
  cd srcdir/test
  ant resolve
  cd srcdir
  ./sandbox/mkjar.sh

Use the run_jni.sh script in srcdir/jnitmp to run jni tests.

