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
                dir('oot') {
                    git url: 'https://github.com/zeldaret/oot.git'
                }
            }
        }
        stage('Set up oot') {
            steps {
                sh 'cd oot'
                sh 'cp /usr/local/etc/roms/baserom_oot.z64 baserom_original.z64'
                sh 'git submodule update --init --recursive'
	            sh '$(MAKE) -C tools'
                sh 'cp ../ZAPD.out tools/ZAPD/'
	            sh 'python3 fixbaserom.py'
	            sh 'python3 extract_baserom.py'
	            sh 'python3 extract_assets.py'
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

