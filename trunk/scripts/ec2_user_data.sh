#!/bin/bash
wget -O- run.alestic.com/install/runurl | bash
runurl run.alestic.com/email/start youremail@example.com
runurl http://aimc.googlecode.com/svn/trunk/scripts/aws_prepare.sh