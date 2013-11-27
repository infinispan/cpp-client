# FindJava and FindJNI don't do much  in cmake 2.6.
# Rework functionality for 2.6 or require 2.8?
# Already done: find_package(Java)
if (NOT JAVA_COMPILE)
    message(FATAL_ERROR "Java javac compiler not found")
endif (NOT JAVA_COMPILE)
if (NOT JAVA_ARCHIVE)
    message(FATAL_ERROR "Java jar archiver not found")
endif (NOT JAVA_ARCHIVE)
message(STATUS "got runtime ${JAVA_RUNTIME}")

find_package(JNI)
if (NOT JNI_LIBRARIES)
    message(FATAL_ERROR "Java JNI support not found")
endif (NOT JNI_LIBRARIES)
message(STATUS "Using JNI libraries: ${JNI_LIBRARIES}" )

find_package(SWIG REQUIRED)
if (NOT SWIG_FOUND)
    message(FATAL_ERROR "SWIG not found")
endif (NOT SWIG_FOUND)
include(UseSWIG)

find_program(ANT_PROGRAM "ant")
if (ANT_PROGRAM STREQUAL "ANT_PROGRAM-NOTFOUND")
    message(FATAL_ERROR "Apache ant not found in path")
else (ANT_PROGRAM STREQUAL "ANT_PROGRAM-NOTFOUND")
    if(WIN32 AND NOT CYGWIN)
        set(ANT_PROGRAM "${ANT_PROGRAM}.bat")
    endif(WIN32 AND NOT CYGWIN)
endif (ANT_PROGRAM STREQUAL "ANT_PROGRAM-NOTFOUND")

if(WIN32 AND NOT CYGWIN)
  set (CLASSPATH_SEPARATOR ";")
else (WIN32 AND NOT CYGWIN)
  set (CLASSPATH_SEPARATOR ":")
endif(WIN32 AND NOT CYGWIN)

SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/jni/src/org/infinispan/client/hotrod/jni)

set(JNI_DIR "${CMAKE_CURRENT_BINARY_DIR}/jni")
if (NOT (EXISTS ${JNI_DIR}))
    file(MAKE_DIRECTORY ${JNI_DIR})
    execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${CMAKE_CURRENT_SOURCE_DIR}/test/ivy.xml" "${JNI_DIR}")
    execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${CMAKE_CURRENT_SOURCE_DIR}/test/ivysettings.xml" "${JNI_DIR}")
    execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${CMAKE_CURRENT_SOURCE_DIR}/test/build.xml" "${JNI_DIR}")
    execute_process(COMMAND "${ANT_PROGRAM}" "resolve" WORKING_DIRECTORY "${JNI_DIR}" OUTPUT_FILE "${JNI_DIR}/resolve.log")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test/swig)
    file(MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR})
endif (NOT (EXISTS ${JNI_DIR}))

SET(CMAKE_SWIG_FLAGS -package org.infinispan.client.hotrod.jni)
SET_SOURCE_FILES_PROPERTIES(test/swig/java.i PROPERTIES CPLUSPLUS ON)

swig_add_module(hotrod-swig java "${CMAKE_CURRENT_SOURCE_DIR}/test/swig/java.i")
include_directories(${JNI_INCLUDE_DIRS})
swig_link_libraries(hotrod-swig hotrod)

set_target_properties(hotrod-swig
    PROPERTIES
    OUTPUT_NAME "hotrod-jni"
    PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}")

if (CMAKE_COMPILER_IS_GNUCXX)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/test/swig/javaJAVA_wrap.cxx
               PROPERTIES COMPILE_FLAGS "-w")
endif (CMAKE_COMPILER_IS_GNUCXX)

file(GLOB_RECURSE JAVA_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/jniapi/*.java)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jni/JniTest.class
    COMMAND ${CMAKE_COMMAND} -DHOTROD_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -D "JAVA_COMPILE=${JAVA_COMPILE}" "-DJAVA_ARCHIVE=${JAVA_ARCHIVE}"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/test/swig/compile_java.cmake
    DEPENDS ${JAVA_SOURCES}
)

add_custom_target(JniTest ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/jni/JniTest.class hotrod-swig)

#For generators with multiple configurations make sure all of the possible target locations are in the java.library.path
set(JAVA_LIBRARY_PATH ".")
foreach(loop_var ${CMAKE_CONFIGURATION_TYPES})
	set(JAVA_LIBRARY_PATH "${JAVA_LIBRARY_PATH}${CLASSPATH_SEPARATOR}${loop_var}")
endforeach(loop_var)

add_test(swig ${JAVA_RUNTIME} 
    -ea 
    "-Djava.library.path=${JAVA_LIBRARY_PATH}" 
    -cp "jni/hotrod-jni.jar${CLASSPATH_SEPARATOR}jni/lib/*${CLASSPATH_SEPARATOR}jni"
    #-agentlib:jdwp=transport=dt_socket,address=8787,server=y,suspend=y  # For remote debugging 
    JniTest
)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/jni/hotrod-jni.jar" DESTINATION jni)
install (TARGETS hotrod-swig LIBRARY DESTINATION jni)
