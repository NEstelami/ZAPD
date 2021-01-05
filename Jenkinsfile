pipeline {
    agent any

    stages {
        stage('Build ZAPD') {
            steps {
                sh 'make -j'
            }
        }
        stage('Checkout oot') {
            steps {
                checkout([
                    $class: 'GitSCM', 
                    branches: [[name: 'master']], 
                    userRemoteConfigs: [[url: 'https://github.com/zeldaret/oot.git']]
                    ]
                )
            }
        }
        stage('Set up oot') {
            steps {
                sh 'cd oot'
                sh 'cp /usr/local/etc/roms/baserom_oot.z64 baserom_original.z64'
                sh 'make -j setup'
                sh 'cp ../ZAPD.out tools/ZAPD/'
            }
        }
        stage('Build oot') {
            steps {
                sh 'make -j'
            }
        }
    }
    post {
        always {
            cleanWs()
        }
    }
}
