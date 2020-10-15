#!/usr/bin/env groovy
pipeline{
    agent none
    stages {
    stage('Build Windows') {
        agent { label 'windows'
        }
        environment {
            cppTag = '8.1.0.Final'
            CMAKE_HOME = 'C:\\\\PROGRA~2\\\\CMake\\\\bin'
            generator = '"Visual Studio 14 2015 Win64"'
            INFINISPAN_VERSION = '9.4.0.Final'
            JAVA_HOME = 'C:\\\\PROGRA~1\\\\JAVA\\\\JDK18~1.0_1'
            M2_HOME = 'C:\\\\APACHE~1.9'
            MVN_PROGRAM = 'C:\\\\APACHE~1.9\\\\BIN\\\\MVN.BAT'
            PROTOBUF_INCLUDE_DIR = 'C:\\\\protobuf-2.6.1-pack\\\\include'
            PROTOBUF_LIBRARY = 'C:\\\\protobuf-2.6.1-pack\\\\lib\\\\libprotobuf-static.lib'
            PROTOBUF_PROTOC_EXECUTABLE = 'C:\\\\protobuf-2.6.1-pack\\\\bin\\\\protoc.exe'
            PROTOBUF_PROTOC_LIBRARY = 'C:\\\\protobuf-2.6.1-pack\\\\lib\\\\libprotoc.lib'
            SWIG_DIR = 'C:\\\\PROGRA~1\\\\SWIGWI~1.12'
            SWIG_EXECUTABLE = 'C:\\\\PROGRA~1\\\\SWIGWI~1.12\\\\SWIG.EXE'
            test32 = 'empty'
            test64 = 'empty'
            HOTROD_LOG_LEVEL = 'INFO'
        }
        steps {
            checkout scm
            script {
                bat 'build.bat'
                archiveArtifacts artifacts: 'build_win\\_CPack_Packages\\WIN-x86_64\\ZIP\\*.zip'
            }
        }
    }
    stage ("Build Linux") {
        agent { label 'slave-group-normal'
        }
        environment {
          HOTROD_LOG_LEVEL = 'INFO'
          INFINISPAN_VERSION = '9.4.0.Final'
          JBOSS_HOME = '/home/ec2-user/ispn/infinispan-server'
          M2_HOME = '/opt/maven'
          PATH = "${M2_HOME}/bin:${PATH}"
          JAVA_HOME = '/etc/alternatives/java_sdk'
        }
        steps {
            checkout scm
            script {
                sh 'printenv'
            }
            script {
                sh "PATH=\"${env.M2_HOME}/bin:${PATH}\" CMAKE_EXTRAS=\"-DINSTALL_GTEST=FALSE\" ./build.sh"
                archiveArtifacts artifacts: 'build/infinispan-hotrod-cpp-*.rpm, build/*Source.zip'
            }
        }
    }
    }
}
