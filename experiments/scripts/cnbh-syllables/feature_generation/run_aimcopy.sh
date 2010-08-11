#!/bin/bash

# Test for the existence of AIMCopy in the path
AIMCOPY=`which AIMCopy`
if [ "$AIMCOPY" == "" ]; then
  echo "Please build AIM-C and make AIMCopy available in the path"
fi

set -e
set -u

FEATURES_DIR=$1
MACHINE_CORES=$2
FILE_LIST=feature_generation_script

echo "Splitting data files over cores..."
total_cores=$(($MACHINE_CORES))
echo -n $total_cores
echo " cores available"
total_files=`cat $FEATURES_DIR/$FILE_LIST | wc -l | sed 's/ *//'`
echo -n $total_files
echo " files to process"
files_per_core=$(($total_files/$total_cores+1))
echo -n $files_per_core
echo " files per core"
split -l $files_per_core $FEATURES_DIR/$FILE_LIST $FEATURES_DIR/split_list
splits=( $(ls $FEATURES_DIR/split_list*))
element=0
echo -n "Spawning "
echo -n $total_cores
echo " tasks..."
for ((c=1;c<=$MACHINE_CORES;c+=1)); do
  s=${splits[$element]}
  AIMCopy -D $FEATURES_DIR/aimcopy_config.out -S $s &
  let element=element+1
done

echo "Waiting for tasks to complete..."
wait
echo "Done!"