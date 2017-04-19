#!/usr/bin/env groovy

pipeline {
    agent {
        label 'windows'
    }
    
    environment {
        
        cppTag = '8.1.0.Final'
        CMAKE_HOME = 'C:\\\\PROGRA~2\\\\CMake\\\\bin'
        generator = '\\"Visual Studio 14 2015 Win64\\"'
        INFINISPAN_VERSION = '9.0.0.Final'
        JAVA_HOME = 'C:\\\\PROGRA~1\\\\JAVA\\\\JDK18~1.0_7'
        M2_HOME = 'C:\\\\APACHE~1.9'
        MVN_PROGRAM = 'C:\\\\APACHE~1.9\\\\BIN\\\\MVN'
        PROTOBUF_INCLUDE_DIR = 'C:\\\\protobuf-2.6.1-pack\\\\include'
        PROTOBUF_LIBRARY = 'C:\\\\protobuf-2.6.1-pack\\\\lib\\\\libprotobuf-static.lib'
        PROTOBUF_PROTOC_EXECUTABLE = 'C:\\\\protobuf-2.6.1-pack\\\\bin\\\\protoc.exe'
        PROTOBUF_PROTOC_LIBRARY = 'C:\\\\protobuf-2.6.1-pack\\\\lib\\\\libprotoc.lib'
        SWIG_DIR = 'C:\\\\PROGRA~1\\\\SWIGWI~1.12'
        SWIG_EXECUTABLE = 'C:\\\\PROGRA~1\\\\SWIGWI~1.12\\\\SWIG.EXE'
        test32 = 'empty'
        test64 = 'empty'
        version_1major = '8'
        version_2minor = '1'
        version_3micro = '0'
        version_4qualifier = 'Final'
  


      HOTROD_LOG_LEVEL = 'TRACE'
      
    }
    
    stages {
        stage('Cleanup') {
            steps{
                step([$class: 'WsCleanup'])
            }
        }
    
        stage('SCM Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Build') {
            steps {
                    script {
                    bat 'build.bat'
                    }
            }
        }
    }
}

