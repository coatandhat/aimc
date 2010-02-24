#!/bin/bash
# Run ami-2fc2e95b (32 bit) or ami-05c2e971 (64 bit) in eu-west zone 
sudo apt-get update
sudo apt-get install subversion scons pkg-config libsndfile-dev build-essential
sudo mkdir /media/sounds-database
sudo mount /dev/sdf1 /media/sounds-database/
svn checkout http://aimc.googlecode.com/svn/trunk/ aimc-read-only
cd aimc-read-only
scons
cd ..
