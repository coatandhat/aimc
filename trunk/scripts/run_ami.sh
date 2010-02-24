#!/bin/bash
ec2-run-instances                               \
  --user-data-file ec2_user_data.sh             \
  --key tom_eu_west                             \
  --instance-type m1.small                      \
  --instance-count 1                            \
  --region eu-west-1                            \
  --availability-zone eu-west-1b                \
  ami-2fc2e95b
  
ec2-attach-volume vol-d6fb18bf -i [INSTANCE_ID] -d /dev/sdf --region eu-west-1