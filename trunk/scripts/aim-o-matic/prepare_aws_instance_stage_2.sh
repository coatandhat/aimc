#!/bin/bash
sudo apt-get -y install scons pkg-config libsndfile1-dev build-essential ffmpeg apache2 php5 libcairo-dev libapache2-mod-php5 graphviz
sudo a2enmod php5
sudo apache2ctl restart
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/../../
scons
sudo cp aimc/scripts/aim-o-matic/aim-o-matic.php aimc/scripts/aim-o-matic/upload-form.html /var/www
echo "*/10 * * * * /home/ubuntu/aimc/scripts/aim-o-matic/aim_o_matic_cronjob.sh" | crontab

