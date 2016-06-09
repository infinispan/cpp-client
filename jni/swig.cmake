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

find_program(MVN_PROGRAM "mvn")
if (MVN_PROGRAM STREQUAL "MVN_PROGRAM-NOTFOUND")
    message(FATAL_ERROR "Apache Maven (mvn) not found in path")
else (MVN_PROGRAM STREQUAL "MVN_PROGRAM-NOTFOUND")
    if(WIN32 AND NOT CYGWIN)
        set(MVN_PROGRAM "${MVN_PROGRAM}.bat")
    endif(WIN32 AND NOT CYGWIN)
endif (MVN_PROGRAM STREQUAL "MVN_PROGRAM-NOTFOUND")

if(WIN32 AND NOT CYGWIN)
  set (CLASSPATH_SEPARATOR ";")
else (WIN32 AND NOT CYGWIN)
  set (CLASSPATH_SEPARATOR ":")
endif(WIN32 AND NOT CYGWIN)

set(JNI_DIR "${CMAKE_CURRENT_BINARY_DIR}/jni")

if(EXISTS ${JNI_DIR})
    file(REMOVE_RECURSE ${JNI_DIR})
endif(EXISTS ${JNI_DIR})
  
file(COPY ${CMAKE_SOURCE_DIR}/jni DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
configure_file(jni/pom.xml ${PROJECT_BINARY_DIR}/jni/pom.xml  @ONLY IMMEDIATE)

set(CMAKE_SWIG_OUTDIR "${JNI_DIR}/src/main/java/org/infinispan/client/hotrod/jni")
set(CMAKE_SWIG_FLAGS -package "org.infinispan.client.hotrod.jni")
set_source_files_properties("jni/src/main/swig/java.i" PROPERTIES CPLUSPLUS ON)

swig_add_module(hotrod-swig java "${CMAKE_CURRENT_SOURCE_DIR}/jni/src/main/swig/java.i")
include_directories(${JNI_INCLUDE_DIRS})
if(DEFINED HOTROD_PREBUILT_LIB_DIR)
    set (INCLUDE_FILES_DIR ${HOTROD_PREBUILT_LIB_DIR}/../include)
    include_directories(${HOTROD_PREBUILT_LIB_DIR}/../include)
    include_directories(${PROTOBUF_INCLUDE_DIR})
endif(DEFINED HOTROD_PREBUILT_LIB_DIR)
swig_link_libraries(hotrod-swig hotrod)

set_target_properties(hotrod-swig
    PROPERTIES
    OUTPUT_NAME "hotrod-jni"
    PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}")
set_target_properties(hotrod-swig PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT}" )

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties("${JNI_DIR}/src/main/swig/javaJAVA_wrap.cxx" PROPERTIES COMPILE_FLAGS "-w -std=c++11")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

file(GLOB_RECURSE JAVA_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/jni *.java)

add_custom_command(OUTPUT ${JNI_DIR}/target/org/infinispan/client/jni/hotrod/JniTest.class
    COMMAND ${MVN_PROGRAM}
    ARGS "-s" "${JNI_DIR}/maven-settings.xml" "package"
    DEPENDS ${JAVA_SOURCES} 
    WORKING_DIRECTORY "${JNI_DIR}" 
)

add_custom_target(JniTest ALL DEPENDS ${JNI_DIR}/target/org/infinispan/client/jni/hotrod/JniTest.class hotrod-swig)

#Target for deploying the jar to the maven repo. Usage: cmake --build . --target JniDeploy
add_custom_target(JniDeploy
  ${MVN_PROGRAM} "-s" "${JNI_DIR}/maven-settings.xml" "deploy"
  DEPENDS ${JNI_DIR}/target/org/infinispan/client/jni/hotrod/JniTest.class hotrod-swig
  WORKING_DIRECTORY "${JNI_DIR}")

#For generators with multiple configurations make sure all of the possible target locations are in the java.library.path
set(JAVA_LIBRARY_PATH ".")
foreach(loop_var ${CMAKE_CONFIGURATION_TYPES})
	set(JAVA_LIBRARY_PATH "${JAVA_LIBRARY_PATH}${CLASSPATH_SEPARATOR}${loop_var}")
endforeach(loop_var)

add_test(swig ${JAVA_RUNTIME} 
    -ea 
    "-Djava.net.preferIPv4Stack=true"
    "-Djava.library.path=${JAVA_LIBRARY_PATH}" 
    -cp "jni/target/hotrod-jni.jar${CLASSPATH_SEPARATOR}jni/target/dependency/*${CLASSPATH_SEPARATOR}jni/target/test-classes"
    #-agentlib:jdwp=transport=dt_socket,address=8787,server=y,suspend=y  # For remote debugging 
    org.infinispan.client.jni.hotrod.JniTest
)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/jni/target/hotrod-jni.jar" DESTINATION jni)
set_property(TARGET hotrod-swig PROPERTY CXX_STANDARD 11)
set_property(TARGET hotrod-swig PROPERTY CXX_STANDARD_REQUIRED ON)
install (TARGETS hotrod-swig LIBRARY DESTINATION jni)
