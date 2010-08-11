#!/bin/bash
#
# Train and test an HTK monophone model using AIM or MFCC
# features and the CNBH syllable databse
#
# Copyright 2009-2010 University of Cambridge
# Author: Thomas Walters <tom@acousticscale.org>
# 
# Run multiple HMMs

set -e
set -u

WORKING_DIRECTORY=$1
FEATURE_SOURCE=$2
FEATURE_SUFFIX=$3
HMM_STATES_LIST=$4
MIXTURE_COMPONENTS_LIST=$5
TRAINING_ITERATIONS_LIST=$6
TESTING_ITERATIONS_LIST=$7
FEATURE_SIZE=$8
FEATURE_TYPE=$9


TRAIN_SPEAKERS=train_speakers
TEST_SPEAKERS=test_speakers

# These are not
SYLLIST_COMPLETE=syllist
GRAM=gram
DICT=dict
WDNET=wdnet
TRAIN_MLF=train.mlf
TEST_MLF=test.mlf
HMMCONFIG=hmmconfig


if [ "$FEATURE_TYPE" == "MFCC_0_D_A"]
then
  cat <<"EOF" > $WORK/$HMMCONFIG
# Coding parameters
SOURCEFORMAT= HTK
EOF
else
  cat <<"EOF" > $WORK/$HMMCONFIG
# Coding parameters
# The TARGETKIND and SOURCEKIND lines are to add deltas and delta-deltas to
# the AIM features
SOURCEFORMAT= HTK
SOURCEKIND= USER_E
TARGETKIND = USER_E_D_A
EOF
fi

for total_hmm_states in 3 4 5 6 7 8; do
  for mixture_components in 1 2 3 4 5 6 7; do
    . run_test_instance.sh &
  done
done
echo "Waiting..."
wait

