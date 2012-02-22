#!/bin/bash
sudo apt-get -y install scons pkg-config libsndfile1-dev build-essential ffmpeg apache2 php5 libcairo-dev libapache2-mod-php5 graphviz
sudo a2enmod php5
sudo apache2ctl restart
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/../../
scons

