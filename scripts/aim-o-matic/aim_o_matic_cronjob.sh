#!/bin/bash

export PATH=$PATH:/usr/local/bin/

INPUT_DIR=/home/ubuntu/upload
DONE_DIR=/home/ubuntu/processed
mkdir ${DONE_DIR}
OUTPUT_DIR=/var/www/results
sudo mkdir $OUTPUT_DIR
sudo chmod o+w $OUTPUT_DIR
AIMC_DIR=/home/ubuntu/aimc/build/posix-release/
AIMC=$AIMC_DIR/AIMCopy

mkdir -p /tmp/aimc/

for t in `seq 1 580`; do
for f in `ls $INPUT_DIR/*.config`; do
  b=`basename $f`
  WORKING_ID=${b%.config}
  THIS_OUTPUT_DIR=${OUTPUT_DIR}/${WORKING_ID}
  OUTPUT_BASE=${THIS_OUTPUT_DIR}/${WORKING_ID}
  mkdir ${THIS_OUTPUT_DIR}
  SCRIPT_FILE=${OUTPUT_BASE}.script
  PARAMS_FILE=${OUTPUT_BASE}.complete_params
  GRAPH_FILE=${OUTPUT_BASE}.dot
  GRAPH_IMAGE=${OUTPUT_BASE}.png
  ext="wav"
  echo ${INPUT_DIR}/${WORKING_ID}.${ext}	${THIS_OUTPUT_DIR}/${WORKING_ID} | cat >> ${SCRIPT_FILE}
  rm /tmp/aimc/*
  $AIMC -C ${f} -S ${SCRIPT_FILE} -D ${PARAMS_FILE} -G ${GRAPH_FILE}
  dot -Tpng -o ${GRAPH_IMAGE} ${GRAPH_FILE}
  sudo chown www-data ${THIS_OUTPUT_DIR}
  mv ${INPUT_DIR}/${WORKING_ID}.* ${DONE_DIR}/
done
sleep 1
done
