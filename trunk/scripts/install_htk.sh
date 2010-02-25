#!/bin/bash
cd /media/sounds-database/htk
wget --user $HTK_USERNAME --password $HTK_PASSWORD http://htk.eng.cam.ac.uk/ftp/software/HTK-3.4.1.tar.gz
tar -xzf HTK-3.4.1.tar.gz
cd htk
./configure --disable-hslab
make
sudo make install