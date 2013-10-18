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

SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/jni/src/org/infinispan/client/hotrod/jni)

set (CMAKE_CXX_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS}")

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
    set_target_properties(hotrod-swig PROPERTIES COMPILE_FLAGS "-pedantic -Wall -Wextra -Wno-unused-function -Wno-shadow -Wno-unused-parameter -Wpointer-arith -Wno-cast-qual -Wcast-align -Wno-long-long -Wvolatile-register-var -Winvalid-pch -Wno-system-headers -Woverloaded-virtual -Wshadow")
endif (CMAKE_COMPILER_IS_GNUCXX)


#add_custom_command(OUTPUT ${CMAKE_SWIG_OUTDIR}
#                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SWIG_OUTDIR}
#                   COMMENT "Creating target directory for hotrod-jni sources")
#add_custom_target(hotrod-jni-swig-out-dir ALL DEPENDS ${CMAKE_SWIG_OUTDIR})
#add_dependencies(hotrod-swig hotrod-jni-swig-out-dir)


add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jni/JniTest.class
    COMMAND ${CMAKE_COMMAND} -DHOTROD_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -D "JAVA_COMPILE=${JAVA_COMPILE}" "-DJAVA_ARCHIVE=${JAVA_ARCHIVE}"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/test/swig/compile_java.cmake
)

add_custom_target(JniTest ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/jni/JniTest.class hotrod-swig)

add_test(swig ${JAVA_RUNTIME} -ea -Djava.library.path=. -cp "jni/hotrod-jni.jar:jni/lib/*:jni" JniTest)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/jni/hotrod-jni.jar" DESTINATION jni)
install (TARGETS hotrod-swig LIBRARY DESTINATION jni)
