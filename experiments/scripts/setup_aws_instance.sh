#!/bin/bash
# Run ami-2fc2e95b (32 bit) or ami-05c2e971 (64 bit) in eu-west zone 
# ec2-run-instances  --user-data-file ec2_user_data.sh \
#                    --key tom_eu_west \ 
#                    --instance-type m1.small  \
#                    --instance-count 1   \
#                    --region eu-west-1   \
#                    --availability-zone eu-west-1b \
#                    ami-2fc2e95b

sudo apt-get -y update
sudo apt-get -y install subversion scons pkg-config libsndfile-dev build-essential libboost-dev libc6-dev-i386 python

sudo mkdir /mnt/aimc
sudo chown ubuntu /mnt/aimc
cd /mnt/aimc
svn checkout http://aimc.googlecode.com/svn/trunk/ aimc-read-only
cd aimc-read-only/experiments
./master.sh
