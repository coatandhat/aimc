#!/bin/bash
# Copyright 2010 Thomas Walters <tom@acousticscale.org>
#
# Run a series of experiments which compare MFCC features generated by HTK to
# AIM features generated using AIM-C using a series of syllable recogntiton
# tasks.
# This script expects to be run from within the AIM-C source tree.
# It builds the HTK binaries and AIM-C AIMCopy binary if they're not
# present.
# The following environment varaibles should be set before this script is run:
# SYLLABLES_DATABASE_URL - URL of a tar file containing the CNBH syllables
# database in FLAC format
# HTK_USERNAME and HTK_PASSWORD - username and password for the site at
# http://htk.eng.cam.ac.uk/
# NUMBER_OF_CORES - total number of machine cores

sudo apt-get -y update
sudo apt-get -y install bc subversion scons pkg-config \
                libsndfile1-dev build-essential libboost-dev \
                python sox python-matplotlib libcairo-dev

# For 64-bit systems, uncomment this line:
sudo apt-get -y install libc6-dev-i386

# Set these to be the location of your input database, and desired output
# locations. (Note: the user running this script needs write permissions on
# the $WORKING_VOLUME.)
WORKING_VOLUME=/mnt/scratch0/aim

SYLLABLES_DATABASE_TAR=$WORKING_VOLUME/001-downloaded_sounds_data/cnbh-syllables.tar
SOUNDS_ROOT=$WORKING_VOLUME/002-sounds/
FEATURES_ROOT=$WORKING_VOLUME/003-features/
HMMS_ROOT=$WORKING_VOLUME/004-hmms/
HTK_ROOT=$WORKING_VOLUME/software/htk/
AIMC_ROOT=$WORKING_VOLUME/software/aimc/

THIS_DIR=`dirname $0`
AIMCOPY_CONFIGURATION_FILE=$THIS_DIR/cnbh-syllables/feature_generation/ssi_profile_features.aimcopycfg

# Number of cores on the experimental machine. Various scripts will try to use
# this if it's set.
# NUMBER_OF_CORES=8

# Fail if any command fails
set -e

# Fail if any variable is unset
set -u

######
# Step 001 - Get the sounds database
if [ ! -e $SYLLABLES_DATABASE_TAR ]; then
  mkdir -p `dirname $SYLLABLES_DATABASE_TAR`
  wget -O $SYLLABLES_DATABASE_TAR $SYLLABLES_DATABASE_URL
fi

if [ ! -d $SOUNDS_ROOT ]; then
  mkdir -p $SOUNDS_ROOT
fi

# Untar the CNBH syllables database, and convert the files from FLAC to WAV.
if [ ! -e $SOUNDS_ROOT/.untar_db_success ]; then
  tar -x -C $SOUNDS_ROOT -f $SYLLABLES_DATABASE_TAR
  touch $SOUNDS_ROOT/.untar_db_success
fi

# Convert the database to .WAV format and place it in $SOUNDS_ROOT/clean
echo "Converting CNBH-syllables database from FLAC to WAV..."
./cnbh-syllables/feature_generation/convert_flac_to_wav.sh $SOUNDS_ROOT
#
######

#####
# Step 002 -
# Generate versions of the CNBH syllables spoke pattern with a range of
# signal-to-noise ratios (SNRs). The versions are put in the directory
# ${SOUNDS_ROOT}/${SNR}_dB/ for each SNR in $SNRS.
SNRS="45"#" 42 39 36 33" #" 30 27 24 21 18 15 12 9 6 3 0"
#SNRS="30" # For testing
./cnbh-syllables/feature_generation/pink_noise.sh $SOUNDS_ROOT/clean/ "$SNRS"

# Make the list of all feature drectories
#FEATURE_DIRS="clean"
FEATURE_DIRS=""
for SNR in $SNRS; do
  FEATURE_DIRS="$FEATURE_DIRS snr_${SNR}dB"
done

# Generate feature sets (for the full range of SNRs in $FEATURE_DIRS)
# 1. Standard MFCC features
# 2. AIM features
# 3. MFCC features with optimal VTLN

if [ ! -d $FEATURES_ROOT ]; then
 mkdir -p $FEATURES_ROOT
fi

if [ ! -e $HTK_ROOT/.htk_installed_success ]; then
  ./HTK/install_htk.sh $HTK_ROOT
fi

if [ ! -e $AIMC_ROOT/.aimc_build_success ]; then
 ./aimc/build_aimc.sh $AIMC_ROOT
fi
export PATH=$PATH:$AIMC_ROOT/build/posix-release/

for SOURCE_SNR in $FEATURE_DIRS; do
  if [ ! -e $FEATURES_ROOT/mfcc/$SOURCE_SNR/.make_mfcc_features_success ]; then
    mkdir -p $FEATURES_ROOT/mfcc/$SOURCE_SNR/
    # Generate the list of files to convert
    ./cnbh-syllables/feature_generation/gen_hcopy_aimcopy_script.sh $FEATURES_ROOT/mfcc/$SOURCE_SNR/ $SOUNDS_ROOT/$SOURCE_SNR/ htk
    # Run the conversion
    ./cnbh-syllables/feature_generation/run_hcopy.sh $FEATURES_ROOT/mfcc/$SOURCE_SNR/ $NUMBER_OF_CORES
    touch $FEATURES_ROOT/mfcc/$SOURCE_SNR/.make_mfcc_features_success
  fi

  if [ ! -e $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/.make_mfcc_vtln_features_success ]; then
    mkdir -p $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/
    # Generate the file list and run the conversion (all one step, since this
    # version uses a different configuration for each talker)
    ./cnbh-syllables/feature_generation/run_mfcc_vtln_conversion.sh $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/ $SOUNDS_ROOT/$SOURCE_SNR/
    touch $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/.make_mfcc_vtln_features_success
  fi

  if [ ! -e $FEATURES_ROOT/aim/$SOURCE_SNR/.make_aim_features_success ]; then
    mkdir -p $FEATURES_ROOT/aim/$SOURCE_SNR/
    ./cnbh-syllables/feature_generation/gen_hcopy_aimcopy_script.sh $FEATURES_ROOT/aim/$SOURCE_SNR/ $SOUNDS_ROOT/$SOURCE_SNR/ ""
    # Run the conversion
    ./cnbh-syllables/feature_generation/run_aimcopy.sh $AIMCOPY_CONFIGURATION_FILE $FEATURES_ROOT/aim/$SOURCE_SNR/ $NUMBER_OF_CORES
    touch $FEATURES_ROOT/aim/$SOURCE_SNR/.make_aim_features_success
  fi
done

mkdir -p $HMMS_ROOT

# Now run a bunch of experiments.
# For each of the feature types, we want to run HMMs with a bunch of
# parameters.
TRAINING_ITERATIONS="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15" # 16 17 18 19 20"
#TESTING_ITERATIONS="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15" #" 16 17 18 19 20"
TESTING_ITERATIONS="15"
#HMM_STATES="3 4 5 6 7 8"
HMM_STATES="4"
#HMM_OUTPUT_COMPONENTS="1 2 3 4 5 6 7"
HMM_OUTPUT_COMPONENTS="4"

run_train_test () {
# TODO(tom): Make sure that the training SNR is generated first
for SOURCE_SNR in $FEATURE_DIRS; do
WORK=$HMMS_ROOT/$FEATURE_CLASS/$FEATURE_SUFFIX/$SOURCE_SNR/$TALKERS/
mkdir -p $WORK
FEATURES_DIR=$FEATURES_ROOT/$FEATURE_CLASS/$SOURCE_SNR/
SPOKE_PATTERN_FILE=`pwd`/cnbh-syllables/run_training_and_testing/train_test_sets/gen_spoke_points/spoke_pattern.txt

./cnbh-syllables/run_training_and_testing/train_test_sets/generate_train_test_lists.sh \
    $TALKERS \
    $WORK \
    $FEATURES_DIR \
    $FEATURE_SUFFIX

TRAINING_SCRIPT=$HMMS_ROOT/$FEATURE_CLASS/$FEATURE_SUFFIX/$TRAINING_SNR/$TALKERS/training_script
TRAINING_MASTER_LABEL_FILE=$HMMS_ROOT/$FEATURE_CLASS/$FEATURE_SUFFIX/$TRAINING_SNR/$TALKERS/training_master_label_file

TESTING_SCRIPT=$WORK/testing_script
TESTING_MASTER_LABEL_FILE=$WORK/testing_master_label_file

./cnbh-syllables/run_training_and_testing/gen_htk_base_files.sh $WORK

./cnbh-syllables/run_training_and_testing/test_features.sh \
    "$WORK" \
    "$FEATURES_ROOT/$FEATURE_CLASS/$SOURCE_SNR/" \
    "$FEATURE_SUFFIX" \
    "$HMM_STATES" \
    "$HMM_OUTPUT_COMPONENTS" \
    "$TRAINING_ITERATIONS" \
    "$TESTING_ITERATIONS" \
    "$FEATURE_SIZE" \
    "$FEATURE_TYPE" \
    "$TRAINING_SCRIPT" \
    "$TESTING_SCRIPT" \
    "$TRAINING_MASTER_LABEL_FILE" \
    "$TESTING_MASTER_LABEL_FILE" \
    "$SPOKE_PATTERN_FILE"
done
}

########################
# Standard MFCCs
FEATURE_CLASS=mfcc
FEATURE_SUFFIX=htk
FEATURE_SIZE=39
FEATURE_TYPE=MFCC_0_D_A
TALKERS=inner_talkers
TRAINING_SNR=clean
run_train_test
########################

########################
# Standard MFCCs
# Train on extrema
FEATURE_CLASS=mfcc
FEATURE_SUFFIX=htk
FEATURE_SIZE=39
FEATURE_TYPE=MFCC_0_D_A
TALKERS=outer_talkers
TRAINING_SNR=clean
run_train_test
########################

########################
# MFCCs with VTLN
FEATURE_CLASS=mfcc_vtln
FEATURE_SUFFIX=htk
FEATURE_SIZE=39
FEATURE_TYPE=MFCC_0_D_A
TALKERS=inner_talkers
TRAINING_SNR=clean
run_train_test
########################

########################
# MFCCs with VTLN
# Train on extrema
FEATURE_CLASS=mfcc_vtln
FEATURE_SUFFIX=htk
FEATURE_SIZE=39
FEATURE_TYPE=MFCC_0_D_A
TALKERS=outer_talkers
TRAINING_SNR=clean
run_train_test
########################

AIM_FEATURE_SUFFIXES="slice_1_no_cutoff ssi_profile_no_cutoff slice_1_cutoff ssi_profile_cutoff smooth_nap_profile"
for f in $AIM_FEATURE_SUFFIXES
do
########################
# AIM Features
# Inner talkers
FEATURE_CLASS=aim
FEATURE_SUFFIX=$f
FEATURE_SIZE=12
FEATURE_TYPE=USER_E_D_A
TALKERS=inner_talkers
TRAINING_SNR=clean
run_train_test
########################

########################
# AIM Features
# Inner talkers
FEATURE_CLASS=aim
FEATURE_SUFFIX=$f
FEATURE_SIZE=12
FEATURE_TYPE=USER_E_D_A
TALKERS=outer_talkers
TRAINING_SNR=clean
run_train_test
########################
done




