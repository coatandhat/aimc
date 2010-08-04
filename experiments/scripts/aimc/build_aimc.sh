#!/bin/bash
# Set up AIM-C on an Amazon EC2 instance. A useful reference for building.
AIMC_DIR=/mnt/experiments/aimc

sudo mkdir -p $AIMC_DIR
sudo chown ubuntu $AIMC_DIR
cd $AIMC_DIR
svn checkout http://aimc.googlecode.com/svn/trunk/ aimc-read-only
cd aimc-read-only
scons
cd ..
export PATH=$PATH:$AIMC_DIR/aimc-read-only/build/posix-release/