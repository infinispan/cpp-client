set(JNI_DIR ${CMAKE_CURRENT_BINARY_DIR}/jni)
file(REMOVE ${JNI_DIR}/source1)
file(REMOVE ${JNI_DIR}/source2)

file(GLOB_RECURSE main_wrappers "${HOTROD_SOURCE_DIR}/test/jniapi/*.java")
foreach(pathname ${main_wrappers})
    file(APPEND "${JNI_DIR}/source1" "${pathname}\n")
endforeach(pathname)

file(GLOB_RECURSE swig_wrappers "${CMAKE_CURRENT_BINARY_DIR}/jni/src/*.java")
foreach(pathname ${swig_wrappers})
    file(APPEND "${JNI_DIR}/source2" "${pathname}\n")
endforeach(pathname)

if(WIN32 AND NOT CYGWIN)
  set (CLASSPATH_SEPARATOR ";")
else (WIN32 AND NOT CYGWIN)
  set (CLASSPATH_SEPARATOR ":")
endif(WIN32 AND NOT CYGWIN)

execute_process(COMMAND ${JAVA_COMPILE} -cp ".${CLASSPATH_SEPARATOR}lib/*" -d . "@source1" "@source2" WORKING_DIRECTORY ${JNI_DIR} RESULT_VARIABLE return_code)
if (NOT ${return_code} EQUAL 0)
   message(FATAL_ERROR "Failed to compile.")
endif(NOT ${return_code} EQUAL 0)

execute_process(COMMAND ${JAVA_ARCHIVE} cf hotrod-jni.jar org WORKING_DIRECTORY ${JNI_DIR} RESULT_VARIABLE return_code)
if (NOT ${return_code} EQUAL 0)
   message(FATAL_ERROR "Failed to build jar.")
endif(NOT ${return_code} EQUAL 0)

execute_process(COMMAND ${JAVA_COMPILE} -cp "hotrod-jni.jar${CLASSPATH_SEPARATOR}lib/*" -d . ${HOTROD_SOURCE_DIR}/test/JniTest.java WORKING_DIRECTORY ${JNI_DIR} RESULT_VARIABLE return_code)
if (NOT ${return_code} EQUAL 0)
   message(FATAL_ERROR "Failed to compile JniTest.")
endif(NOT ${return_code} EQUAL 0)

if(NOT EXISTS ${JNI_DIR}/JniTest.class)
    message(FATAL_ERROR "JniTest compile failed")
endif(NOT EXISTS ${JNI_DIR}/JniTest.class)
