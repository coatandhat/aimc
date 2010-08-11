#!/bin/bash
# Copyright 2010 Thomas Walters <tom@acousticscale.org>
#
# Run a series of experiments which compare MFCC features generated by HTK to
# AIM features generated using AIM-C using a series of syllable recogntiton
# tasks.
# This script expects the HTK binaries and AIM-C AIMCopy binary to be present
# in the PATH.

# Set these to be the location of your input database, and desired output
# locations.
SYLLABLES_DATABASE_TAR=/media/sounds/cnbh-syllables.tar
SOUNDS_ROOT=/mnt/experiments/sounds/
FEATURES_ROOT=/mnt/experiments/features/
HMMS_ROOT=/mnt/experiments/hmms/

# Number of cores on the experimental machine. Various scripts will try to use
# this if it's set.
NUMBER_OF_CORES=2

# Fail if any command fails
set -e

# Fail if any variable is unset
set -u

if [ ! -d $SOUNDS_ROOT ]; then
  sudo mkdir -p $SOUNDS_ROOT
  sudo chown `whoami` $SOUNDS_ROOT
fi

# Untar the CNBH syllables database, and convert the files from FLAC to WAV
if [ ! -e $SOUNDS_ROOT/.untar_db_success ]; then
  tar -x -C $SOUNDS_ROOT -f $SYLLABLES_DATABASE_TAR
  touch $SOUNDS_ROOT/.untar_db_success
fi

# Convert the database to .WAV format and place it in $SOUNDS_ROOT/clean
./cnbh-syllables/feature_generation/convert_flac_to_wav.sh $SOUNDS_ROOT

# Generate versions of the CNBH syllables spoke pattern with a range of
# signal-to-noise ratios (SNRs). The versions are put in the directory
# ${SOUNDS_ROOT}/${SNR}_dB/ for each SNR in $SNRS.
SNRS="30 27 24 21 18 15 12 9 6 3 0"
./cnbh-syllables/feature_generation/pink_noise.sh $SOUNDS_ROOT/clean/ $SNRS

# Make the list of all feature drectories
FEATURE_DIRS="clean"
for SNR in $SNRS; do
  FEATURE_DIRS="$FEATURE_DIRS snr_${SNR}dB"
done

# Generate feature sets (for the full range of SNRs in $FEATURE_DIRS)
# 1. Standard MFCC features
# 2. AIM features
# 3. MFCC features with optimal VTLN 


if [ ! -d $FEATURES_ROOT ]; then
  sudo mkdir -p $FEATURES_ROOT
  sudo chown `whoami` $FEATURES_ROOT
fi

for SOURCE_SNR in $FEATURE_DIRS; do
  
  if [ ! -e $FEATURES_ROOT/mfcc/$SOURCE_SNR/.make_mfcc_features_success]
  then
    mkdir -p $FEATURES_ROOT/mfcc/$SOURCE_SNR/
    # Generate the list of files to convert
    ./cnbh-syllables/feature_generation/gen_hcopy_aimcopy_script.sh $FEATURES_ROOT/mfcc/$SOURCE_SNR/ $SOUNDS_ROOT/$SOURCE_SNR/
    # Run the conversion
    ./cnbh-syllables/feature_generation/run_hcopy.sh $FEATURES_ROOT/mfcc/$SOURCE_SNR/ $NUMBER_OF_CORES
    touch $FEATURES_ROOT/mfcc/$SOURCE_SNR/.make_mfcc_features_success
  done

  if [ ! -e $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/.make_mfcc_vtln_features_success]
  then
    mkdir -p $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/
    # Generate the file list and run the conversion (all one step, since this
    # version uses a different configuraiton for each talker)
    ./cnbh-syllables/feature_generation/run_mfcc_vtln_conversion.sh $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/ $SOUNDS_ROOT/$SOURCE_SNR/
    touch $FEATURES_ROOT/mfcc_vtln/$SOURCE_SNR/.make_mfcc_vtln_features_success
  done

  if [ ! -e $FEATURES_ROOT/aim/$SOURCE_SNR/.make_aim_features_success]
  then
    mkdir -p $FEATURES_ROOT/aim/$SOURCE_SNR/ 
    ./cnbh-syllables/feature_generation/gen_hcopy_aimcopy_script.sh $FEATURES_ROOT/aim/$SOURCE_SNR/ $SOUNDS_ROOT/$SOURCE_SNR/
    # Run the conversion
    ./cnbh-syllables/feature_generation/run_aimcopy.sh $FEATURES_ROOT/aim/$SOURCE_SNR/ $NUMBER_OF_CORES
    touch $FEATURES_ROOT/aim/$SOURCE_SNR/.make_aim_features_success
  done
done 

# Now run a bunch of experiments.
# For each of the feature types, we want to run HMMs with a bunch of
# parameters.
TRAINING_ITERATIONS="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"
TESTING_ITERATIONS="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"
HMM_STATES="3 4 5 6 7 8"
HMM_OUTPUT_COMPONENTS=""


