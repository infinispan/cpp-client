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

On Mac OS X, use Brew or another package manager to install gcc and swig.
The compilation will not currently work with Apple's LLVM-based compilers.

* http://brew.sh/

On Windows, Visual Studio 2008 + SP1, or any newer version, is
required.  You can obtain Windows installable distributions of other
needed software from:

* http://www.python.org/download/releases/
* http://www.swig.org/download.html
* http://www.cmake.org/cmake/resources/software.html

To build (where "srcdir" is the directory containg this README):

    os_make_directory my_build  [arbitrary location]
    cd my_build
    cmake [-G <generator>] [-DHOTROD_JBOSS_HOME=/path/to/ispn/dist] path/to/srcdir
    cmake --build . [--config <configuration>]
    ctest [-V] [-C <configuration>]
    cpack [-G <cpack_generator>] [-C <configuration>]

On windows platforms:

* to build the 64bit binaries you need to specify a 64bit generator using
the -G cmake flag (e.g. -G "Visual Studio 12 Win64").
* you can specify a custom configuration (e.g. Debug/Release/RelWithDebInfo...)
by passing the --config flag during the build phase (you will need to pass
the same configuration to ctest and cpack using the -C flag).

Please check the cmake/ctest/cpack documentation for details.

Platforms with Valgrind may have additional targets:

    cmake --build . --target memtest
    cmake --build . --target memtestv

## Reporting Issues ##
Infinispan uses JIRA for issue management, hosted on issues.jboss.org
(https://issues.jboss.org/browse/HRCPP). You can log in using your jboss.org
username and password.