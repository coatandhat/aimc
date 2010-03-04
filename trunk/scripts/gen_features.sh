#!/bin/bash
#
# Run the syllables database 'spider pattern' through AIM-C
#
# Copyright 2009-2010 University of Cambridge
# Author: Thomas Walters <tom@acousticscale.org>

# Source directory for all the sound files
#SOUND_SOURCE="/Users/Tom/Documents/Work/PhD/HTk-AIM/Sounds/"
#SOUND_SOURCE="/media/sounds-database/cnbh-sounds/"
SOUND_SOURCE="/mnt/sounds/snr_+0dB/"

# Location of the AIMCopy binary if not in the path
AIMCOPY_PREFIX="../aimc-read-only/build/posix-release/"

MACHINE_CORES=8

# Set to true / 1 to enable MFCC features rather than AIM features
# (leave blank for AIM features)
MFCC_FEATURES=

# Names of various internal files and directories. 
# Rename here if you don't like them for some reason.
SYLLIST=syls
TRAIN_SPEAKERS=train_speakers
TEST_SPEAKERS=test_speakers
WORK_PREFIX=snr
TRAIN_LIST=train.list
TEST_LIST=test.list
COMBINED_LIST=combined.list
FEATURES_DIR=features
AIMCOPY_CONFIG=aimcopy.cfg
HCOPY_CONFIG=hcopy.cfg
AIMCOPY_LOG_TRAIN=aimcopy_train.log
AIMCOPY_LOG_TEST=aimcopy_test.log
HTK_PREFIX=""


# The vowels and consonants that make up the CNBH database
VOWELS="a e i o u"
CONSONANTS="b d f g h k l m n p r s t v w x y z"
SILENCE="sil"

WORK=${WORK_PREFIX}`echo $1 | tr -d ' '`
mkdir -p $WORK

# Make a copy of this script in the experimental directory
cp -p $0  $WORK


echo "Generating syllable list..."

for v in $VOWELS; do
  echo $v$v >> $WORK/$SYLLIST.tmp
  for c in $CONSONANTS; do
    echo $v$c >> $WORK/$SYLLIST.tmp
    echo $c$v >> $WORK/$SYLLIST.tmp
  done
done

# Sort the syllable list and delete the 
# temporary, unsorted version
sort $WORK/$SYLLIST.tmp > $WORK/$SYLLIST


# Generate a list of filenames from the spoke pattern
cat <<"EOF" > $WORK/$TRAIN_SPEAKERS
170.9p112.2s100.0t+000itd
171.0p112.8s100.0t+000itd	
171.3p111.7s100.0t+000itd	
171.5p113.1s100.0t+000itd	
171.9p111.5s100.0t+000itd	
172.1p113.0s100.0t+000itd	
172.4p111.9s100.0t+000itd	
172.5p112.5s100.0t+000itd
EOF

cat <<"EOF" > $WORK/$TEST_SPEAKERS
137.0p104.3s100.0t+000itd	
141.3p135.4s100.0t+000itd	
145.5p106.3s100.0t+000itd	
148.8p128.8s100.0t+000itd	
151.6p83.9s100.0t+000itd	
153.0p108.1s100.0t+000itd	
155.5p123.5s100.0t+000itd	
156.7p90.6s100.0t+000itd	
159.5p109.6s100.0t+000itd	
161.1p119.4s100.0t+000itd	
161.1p96.8s100.0t+000itd	
163.4p157.6s100.0t+000itd	
164.7p110.8s100.0t+000itd	
164.9p102.1s100.0t+000itd	
165.6p144.0s100.0t+000itd	
165.7p116.2s100.0t+000itd	
167.4p133.5s100.0t+000itd	
167.8p106.5s100.0t+000itd	
168.6p111.6s100.0t+000itd	
168.9p125.4s100.0t+000itd	
169.0p114.0s100.0t+000itd	
170.0p109.7s100.0t+000itd	
170.1p119.5s100.0t+000itd	
171.0p115.5s100.0t+000itd	
171.7p112.3s100.0t+000itd	
172.4p109.3s100.0t+000itd	
173.3p105.6s100.0t+000itd	
173.5p115.0s100.0t+000itd	
174.5p100.6s100.0t+000itd	
174.5p110.6s100.0t+000itd	
174.9p113.0s100.0t+000itd	
175.7p118.5s100.0t+000itd	
176.1p94.5s100.0t+000itd	
178.0p108.5s100.0t+000itd	
178.1p87.6s100.0t+000itd	
178.8p123.6s100.0t+000itd	
179.0p113.9s100.0t+000itd	
180.4p80.1s100.0t+000itd	
183.0p105.7s100.0t+000itd	
183.0p130.4s100.0t+000itd	
184.8p115.1s100.0t+000itd	
188.1p139.2s100.0t+000itd	
189.6p102.1s100.0t+000itd	
192.7p116.7s100.0t+000itd	
194.5p150.4s100.0t+000itd	
198.1p97.9s100.0t+000itd	
202.7p118.6s100.0t+000itd	
208.6p93.2s100.0t+000itd	
215.2p121.0s100.0t+000itd
EOF

# Construct the conversion scripts for AIMCopy (or HCopy) and 
# the master label files for the train and test sets

echo "Generating train and test scripts..."
if [ -a $WORK/$TRAIN_LIST ] 
then
  rm $WORK/$TRAIN_LIST
fi
if [ -a $WORK/$TEST_LIST ]
then
  rm $WORK/$TEST_LIST
fi  
 
exec 3> $WORK/$TRAIN_LIST
exec 5> $WORK/$TEST_LIST
for syllable in $(cat $WORK/$SYLLIST); do
  for speaker in $(cat $WORK/$TRAIN_SPEAKERS); do
    SOURCE_FILENAME=$SOUND_SOURCE/$syllable/${syllable}${speaker}.wav
    DEST_FILENAME=$WORK/$FEATURES_DIR/$syllable/${syllable}${speaker}
    echo "$SOURCE_FILENAME  ${DEST_FILENAME}.htk" >&3
  done
  for speaker in $(cat $WORK/$TEST_SPEAKERS); do
    SOURCE_FILENAME=$SOUND_SOURCE/$syllable/${syllable}${speaker}.wav
    DEST_FILENAME=$WORK/$FEATURES_DIR/$syllable/${syllable}${speaker}
    echo "$SOURCE_FILENAME  ${DEST_FILENAME}.htk" >&5
  done
done
exec 3>&-
exec 5>&-

# Make the necessary directories for the computed features
echo "Making directory structure..."
mkdir $WORK/$FEATURES_DIR
for syllable in $(cat $WORK/$SYLLIST); do
  mkdir $WORK/$FEATURES_DIR/$syllable
done

# Write the AIMCopy config file
echo "Creating AIMCopy config file..."
cat <<"EOF" > $WORK/$AIMCOPY_CONFIG
input.buffersize=480
sai.frame_period_ms=10.0
slice.normalize=true
ssi.weight_by_cutoff=false
ssi.weight_by_scaling=true
ssi.log_cycles_axis=true
EOF
echo "noise.level_db=$1" >> $WORK/$AIMCOPY_CONFIG

echo "Creating HCopy config file..."
cat <<"EOF" > $WORK/$HCOPY_CONFIG
# Coding parameters
SOURCEFORMAT= WAV
TARGETKIND = MFCC_0_D_A
TARGETRATE = 100000.0
SAVECOMPRESSED = T
SAVEWITHCRC = T
WINDOWSIZE = 250000.0
USEHAMMING = T
PREEMCOEF = 0.97
NUMCHANS = 200
CEPLIFTER = 22
NUMCEPS = 12
ENORMALISE = F
EOF
 
echo "Splitting data files..."
cat $WORK/${TRAIN_LIST} $WORK/${TEST_LIST} > $WORK/${COMBINED_LIST}

total_cores=$(($MACHINE_CORES))
echo -n $total_cores
echo " cores available"
total_files=`cat $WORK/${COMBINED_LIST} | wc -l | sed 's/ *//'`
echo -n $total_files
echo " files to process"
files_per_core=$(($total_files/$total_cores+1))
echo -n $files_per_core
echo " files per core"
split -l $files_per_core $WORK/${COMBINED_LIST} $WORK/split_list
splits=( $(ls $WORK/split_list*))
element=0
echo "Spawning tasks..."
for ((c=1;c<=$MACHINE_CORES;c+=1)); do
  s=${splits[$element]}
  if [ "$MFCC_FEATURES" ]
  then
    ${HTK_PREFIX}HCopy -T 1 -C $WORK/$HCOPY_CONFIG -S $s &
  else
    ${AIMCOPY_PREFIX}AIMCopy -C $WORK/$AIMCOPY_CONFIG -S $s &
  fi
  let element=element+1
done

echo "Waiting for tasks to complete..."
wait
echo "Done!"
