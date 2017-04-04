#!/usr/bin/env groovy

pipeline {
    agent any
    stage('Cleanup') {
        steps{
            step([$class: 'WsCleanup'])
        }
    }
    
    stages {
        stage('SCM Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Build') {
            steps {
                sh ./build.sh
            }
        }
    }
}
