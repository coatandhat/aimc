#!/bin/bash
# Run ami-2fc2e95b (32 bit) or ami-05c2e971 (64 bit) in eu-west zone 
sudo apt-get -y update
sudo apt-get -y install subversion scons pkg-config libsndfile-dev build-essential libboost-dev
sudo mkdir /media/sounds-database
sudo mkdir /media/results
sudo mount /dev/sdf1 /media/sounds-database/
sudo mount /dev/sdg1 /media/results/
#cd /media/sounds-database/htk/htk/
#sudo make install
su ubuntu
cd /mnt/
sudo mkdir work
sudo chown `whoami` work
cd work
svn checkout http://aimc.googlecode.com/svn/trunk/ aimc-read-only
cd aimc-read-only
scons
cd ..
mkdir experiments
cd experiments
cp ../aimc-read-only/scripts/gen_features.sh ./
chmod u+x gen_features.sh
screen ./gen_features.sh $SNR_VALUE
tar -cf snr$SNR_VALUE.tar snr$SNR_VALUE/
cp snr$SNR_VALUE.tar /media/results/snr/
#sudo shutdown -h now