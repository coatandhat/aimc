#!/bin/bash
sudo apt-get -y update
sudo apt-get -y install subversion
svn checkout http://aimc.googlecode.com/svn/trunk/ aimc
aimc/scripts/aim-o-matic/prepare_aws_instance_stage_2.sh

