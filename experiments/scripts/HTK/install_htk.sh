#!/bin/bash
set -e
set -u
if [ ! -e /mnt/experiments/htk/.htk_installed_success ]; then
sudo mkdir -p /mnt/experiments/htk
sudo chown `whoami` /mnt/experiments/htk
cd /mnt/experiments/htk
wget --user $HTK_USERNAME --password $HTK_PASSWORD http://htk.eng.cam.ac.uk/ftp/software/HTK-3.4.1.tar.gz
tar -xzf HTK-3.4.1.tar.gz
cd htk
./configure --disable-hslab
make
sudo make install
touch /mnt/experiments/htk/.htk_installed_success
fi