# cpp-client #

C++ client for Infinispan, over the [Hot Rod] (https://docs.jboss.org/author/display/ISPN/Hot+Rod+Protocol) wire protocol.
Released under Apache License, Version 2.0.

## Build ##

Build prerequisites: CMake, swig, java, C++

To build (where "srcdir" is the directory containg this README):

  os_make_directory my_build  [arbitrary location]
  cd my_build
  cmake path/to/srcdir
  ctest

The Java swig module is experimental.  See sandbox/mkjar.sh for
building the JNI libraries.
