# C++ Hot Rod Client #

C++ client for Infinispan, over the [Hot Rod] (https://docs.jboss.org/author/display/ISPN/Hot+Rod+Protocol) wire protocol.
Released under Apache License, Version 2.0.

## Documentation
For more information, please refer to [the documentation of this client library](documentation/index.adoc).

## Build ##
Build prerequisites: 

* CMake 2.8
* C++03 plus shared_ptr TR1 support.
* Infinispan server distribution (located via JBOSS_HOME or cmake directive)
* Java (any version compatible with the Infinispan distribution)
* Python 2.6 or newer
  
optional: Doxygen

Components needed to use swig tests: 
* Apache Maven 3.0.x (http://maven.apache.org/)
* SWIG 2.0.x (http://www.swig.org/)

On Linux, use the package manager for your distribution to install the
necessary packages.  You may have to point to a special repository,
such as EPEL, to get CMake 2.8.

On Windows, Visual Studio 2008 + SP1, or any newer version, is
required.  You can obtain Windows installable distributions of other
needed software from:

* http://www.python.org/download/releases/
* http://www.swig.org/download.html
* http://www.cmake.org/cmake/resources/software.html

To build (where "srcdir" is the directory containg this README):

    os_make_directory my_build  [arbitrary location]
    cd my_build
    cmake [-DHOTROD_JBOSS_HOME=/path/to/ispn/dist] path/to/srcdir
    cmake --build .
    ctest [-V]

Platforms with makefiles and Valgrind may have additional targets:

    make test
    make memtest
    make memtestv
