# Infinispan HotRod C++ Client

This package contains the Infinispan HotRod C++ Client

The package directory structure is as follows:

- include                 The header files for the HotRod library
- lib 
   |- libhotrod.so        The shared HotRod library
- jni
   |- libhotrod-jni.so    A JNI wrapper around the HotRod library which enables testing using the Infinispan HotRod client testsuite
   |- hotrod-jni.jar      Java wrapper classes for the JNI library
