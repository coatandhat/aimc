#!/bin/bash
# using getopts
#
# Train and test an HTK monophone model using AIM
# features and the CNBH syllable databse
#
# Copyright 2009-2010 University of Cambridge
# Author: Thomas Walters <tom@acousticscale.org>
# Based on the MATLAB scripts by Jess Monaghan and 
# modelled on HTKTimit.sh from Cantab Research

skip_features=
skip_init=
matlab_plot=
while getopts 'fim' OPTION
do
  case $OPTION in
  f)	skip_features=1
		;;
	i)	skip_init=1
  	;;
  m)  matlab_plot=1
    ;;
#  b)	bflag=1
#		bval="$OPTARG"
#		;;
  ?)	printf "Usage: %s: [-f] [-i] [-m] args\n" $(basename $0) >&2
		exit 2
		;;
  esac
done
shift $(($OPTIND - 1))

# Machine list
USE_MULTIPLE_MACHINES=
MACHINE_LIST="db-xserve2 db-xserve3 db-xserve5 db-xserve6 db-xserve7 db-xserve8"
MACHINE_COUNT=`echo $MACHINE_LIST | wc -w | sed 's/ *//'`
# Cores per machine
MACHINE_CORES=4

# Set to true / 1 to enable MFCC features rather than AIM features
# (leave blank for AIM features)
MFCC_FEATURES=

# Source directory for all the sound files
SOUND_SOURCE="/media/sounds-database/cnbh-sounds"

# Location of the AIMCopy binary if not in the path
AIMCOPY_PREFIX="../aimc-read-only/build/posix-release/"

# Location of HTK binaries if not in the path
HTK_PREFIX=""

# Names of various internal files and directories. 
# Rename here if you don't like them for some reason.
SYLLIST=syls
SYLLIST_COMPLETE=syllist
GRAM=gram
DICT=dict
WDNET=wdnet
TRAIN_SPEAKERS=train_speakers
TEST_SPEAKERS=test_speakers
WORK_PREFIX=work
TRAIN_LIST=train.list
TEST_LIST=test.list
COMBINED_LIST=combined.list
TRAIN_MLF=train.mlf
TEST_MLF=test.mlf
TRAIN_SCRIPT=train.scp
TEST_SCRIPT=test.scp
FEATURES_DIR=features
AIMCOPY_CONFIG=aimcopy.cfg
AIMCOPY_LOG_TRAIN=aimcopy_train.log
AIMCOPY_LOG_TEST=aimcopy_test.log
HCOPY_CONFIG=hcopy.cfg
HMMCONFIG=hmmconfig
HMMPROTO=proto
RECOUT=recout.mlf
RESULTS_FILE=results.txt
MISCLASSIFIED=misclassified.txt
HHED_SCRIPT=cmdscript

# The vowels and consonants that make up the CNBH database
VOWELS="a e i o u"
CONSONANTS="b d f g h k l m n p r s t v w x y z"
SILENCE="sil"

WORK=${WORK_PREFIX}`echo $1 | tr -d ' '`
mkdir -p $WORK

# Make a copy of this script in the experimental directory
cp -p $0  $WORK

if [ "$skip_init" ]
then
  echo "Skipping initialisation"
else
  # Make the sets of VC, CV, and vowel only labels, plus silence and use them to
  # generate the grammar, dictionary and list of syllables
  echo "Generating grammar, dictionary and syllable list..."
  echo -n '$word = ' > $WORK/$GRAM
  FIRST=true;
  for v in $VOWELS; do
    echo $v$v >> $WORK/$SYLLIST.tmp
    echo "$v$v [$v$v] $v$v" >> $WORK/$DICT.tmp
    if $FIRST; then
      echo -n "$v$v" >> $WORK/$GRAM
      FIRST=false
    else
      echo -n " | $v$v" >> $WORK/$GRAM
    fi
    for c in $CONSONANTS; do
      echo $v$c >> $WORK/$SYLLIST.tmp
      echo "$v$c [$v$c] $v$c" >> $WORK/$DICT.tmp 
      echo -n " | $v$c" >> $WORK/$GRAM
      echo $c$v >> $WORK/$SYLLIST.tmp
      echo "$c$v [$c$v] $c$v" >> $WORK/$DICT.tmp 
      echo -n " | $c$v" >> $WORK/$GRAM
    done
  done
  echo ';' >> $WORK/$GRAM

  # Sort the syllable list and the dictionary and delete the 
  # temporary, unsorted version
  sort $WORK/$SYLLIST.tmp > $WORK/$SYLLIST
  rm $WORK/$SYLLIST.tmp
  sort $WORK/$DICT.tmp > $WORK/$DICT
  rm $WORK/$DICT.tmp

  # Add silence to the end of the various files just generated
  cp $WORK/$SYLLIST $WORK/$SYLLIST_COMPLETE
  echo $SILENCE >> $WORK/$SYLLIST_COMPLETE
  echo "end_$SILENCE [$SILENCE] $SILENCE" >> $WORK/$DICT
  echo "start_$SILENCE [$SILENCE] $SILENCE" >> $WORK/$DICT
  echo "(  start_$SILENCE   \$word   end_$SILENCE  )" >> $WORK/$GRAM

  # Use HParse to parse the grammar into a wordnet
  echo "Generating wordnet from grammar..."
  ${HTK_PREFIX}HParse $WORK/$GRAM $WORK/$WDNET

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

  echo "Generating train and test scripts and master label files..."
  exec 4> $WORK/$TRAIN_MLF
  exec 6> $WORK/$TEST_MLF
  echo '#!MLF!#' >&4
  echo '#!MLF!#' >&6
  if [ -a $WORK/$TRAIN_LIST ] 
  then
    rm $WORK/$TRAIN_LIST
  fi
  if [ -a $WORK/$TEST_LIST ]
  then
    rm $WORK/$TEST_LIST
  fi  
  if [ -a $WORK/$TRAIN_SCRIPT ]
  then
    rm $WORK/$TRAIN_SCRIPT
  fi
  if [ -a $WORK/$TEST_SCRIPT ]
  then
    rm $WORK/$TEST_SCRIPT
  fi  
  exec 3> $WORK/$TRAIN_LIST
  exec 5> $WORK/$TEST_LIST
  exec 7> $WORK/$TRAIN_SCRIPT
  exec 8> $WORK/$TEST_SCRIPT
  for syllable in $(cat $WORK/$SYLLIST); do
    for speaker in $(cat $WORK/$TRAIN_SPEAKERS); do
      SOURCE_FILENAME=$SOUND_SOURCE/$syllable/${syllable}${speaker}.wav
      DEST_FILENAME=$WORK/$FEATURES_DIR/$syllable/${syllable}${speaker}
      echo "$SOURCE_FILENAME  ${DEST_FILENAME}.htk" >&3
      echo "'${DEST_FILENAME}.htk'" >&7
      echo "'\"${DEST_FILENAME}.lab\"'" >&4
      echo "$SILENCE" >&4
      echo $syllable >&4
      echo "$SILENCE" >&4
      echo "." >&4
    done
    for speaker in $(cat $WORK/$TEST_SPEAKERS); do
      SOURCE_FILENAME=$SOUND_SOURCE/$syllable/${syllable}${speaker}.wav
      DEST_FILENAME=$WORK/$FEATURES_DIR/$syllable/${syllable}${speaker}
      echo "$SOURCE_FILENAME  ${DEST_FILENAME}.htk" >&5
      echo "'${DEST_FILENAME}.htk'" >&8
      echo "'\"${DEST_FILENAME}.lab\"'" >&6
      echo "$SILENCE" >&6
      echo $syllable >&6
      echo "$SILENCE" >&6
      echo "." >&6
    done
  done
  exec 3>&-
  exec 4>&-
  exec 5>&-
  exec 6>&-
  exec 7>&-
  exec 8>&-
fi

if [ "$skip_features" ]
then
  echo "Skipping feature generation"
else
  # Make the necessary directories for the computed features
  echo "Making directory structure..."
  mkdir $WORK/$FEATURES_DIR
  for syllable in $(cat $WORK/$SYLLIST); do
    mkdir $WORK/$FEATURES_DIR/$syllable
  done

  if [ "$MFCC_FEATURES" ]
  then
    # Write the HCopy config file
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
    echo "Generating features for training..."
    ${HTK_PREFIX}HCopy -T 1 -C $WORK/$HCOPY_CONFIG -S $WORK/${TRAIN_LIST}

    echo "Generating features for testing..."
    ${HTK_PREFIX}HCopy -T 1 -C $WORK/$HCOPY_CONFIG -S $WORK/${TEST_LIST}
  else
    # Write the AIMCopy config file
    echo "Creating AIMCopy config file..."
    cat <<"EOF" > $WORK/$AIMCOPY_CONFIG
input.buffersize=480
gtfb.channel_count=200
gtfb.min_frequency=86.0
gtfb.max_frequency=16000.0
nap.do_lowpass=true
nap.lowpass_cutoff=100.0
slice.temporal=false
slice.all=true
slice.normalize=true
EOF
    if [ "$USE_MULTIPLE_MACHINES" ]
    then
      echo "Splitting data files..."
      cat $WORK/${TRAIN_LIST} $WORK/${TEST_LIST} > $WORK/${COMBINED_LIST}
      total_cores=$(($MACHINE_COUNT*$MACHINE_CORES))
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
      for m in $MACHINE_LIST; do
        for ((c=1;c<=$MACHINE_CORES;c+=1)); do
          s=${splits[$element]}
          echo "ssh $m \"cd HTK-AIM;${AIMCOPY_PREFIX}AIMCopy -C $WORK/$AIMCOPY_CONFIG -S $s\" &" 
          #ssh $m "cd HTK-AIM;${AIMCOPY_PREFIX}AIMCopy -C $WORK/$AIMCOPY_CONFIG -S $s" &
          let element=element+1
        done
      done
      echo "Waiting for tasks to complete..."
      wait
    else
      echo "Generating features for training..."
      ${AIMCOPY_PREFIX}AIMCopy -C $WORK/$AIMCOPY_CONFIG -S $WORK/${TRAIN_LIST} -D $WORK/$AIMCOPY_LOG_TRAIN

      echo "Generating features for testing..."
      ${AIMCOPY_PREFIX}AIMCopy -C $WORK/$AIMCOPY_CONFIG -S $WORK/${TEST_LIST} -D $WORK/$AIMCOPY_LOG_TEST
    fi
  fi
fi

if [ "$MFCC_FEATURES" ]
then
  cat <<"EOF" > $WORK/$HMMCONFIG
# Coding parameters
SOURCEFORMAT= HTK
EOF
else
  cat <<"EOF" > $WORK/$HMMCONFIG
# Coding parameters
SOURCEFORMAT= HTK
SOURCEKIND= USER_E
TARGETKIND = USER_E_D_A
EOF
fi

echo "Creating HMM structure..."
if [ "$MFCC_FEATURES" ]
then
  cat <<"EOF" > $WORK/$HMMPROTO
~o<VECSIZE> 39<NULLD><MFCC_0_D_A>
~h "proto"
<BEGINHMM>
<NUMSTATES> 6
<State> 2
<Mean>39
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
<Variance> 39
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<State> 3
<Mean>39
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
<Variance> 39
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<State> 4
<Mean>39
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
<Variance> 39
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<State> 5
<Mean>39
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
<Variance> 39
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0

<TransP> 6
0.0 1.0 0.0 0.0 0.0 0.0
0.0 0.6 0.4 0.0 0.0 0.0
0.0 0.0 0.6 0.4 0.0 0.0
0.0 0.0 0.0 0.6 0.4 0.0
0.0 0.0 0.0 0.0 0.6 0.4
0.0 0.0 0.0 0.0 0.0 0.0
<EndHMM>
EOF
else
    cat <<"EOF" > $WORK/$HMMPROTO
~o<VECSIZE> 12<NULLD><USER_E_D_A>
~h "proto"
<BEGINHMM>
<NUMSTATES> 4
<State> 2
<Mean>12
0 0 0 0 0 0 0 0 0 0 0 0
<Variance> 12
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<State> 3
<Mean>12
0     0     0     0     0     0     0     0     0  0     0     0
<Variance> 12
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<TransP> 4
0.0 1.0 0.0 0.0
0.0 0.6 0.4 0.0
0.0 0.0 0.6 0.4
0.0 0.0 0.0 0.0
<EndHMM>
EOF
fi


echo "Training HMM..."
echo "Setting up prototype HMM..."
mkdir $WORK/hmm0
${HTK_PREFIX}HCompV -C $WORK/$HMMCONFIG -f 0.01 -m -S $WORK/$TRAIN_SCRIPT -M $WORK/hmm0 $WORK/$HMMPROTO

echo "Generating HMM definitions..."
# Now take the prototype file from hmm0, and create the other HMM definitions from it
grep -A 9999 "<BEGINHMM>" $WORK/hmm0/$HMMPROTO > $WORK/hmm0/hmms
for syllable in $(cat $WORK/$SYLLIST_COMPLETE); do
  echo "~h $syllable" >> $WORK/hmm0/hmmdefs
  cat $WORK/hmm0/hmms >> $WORK/hmm0/hmmdefs
done
if [ "$MFCC_FEATURES" ]
then
  echo -n "~o<STREAMINFO> 1 39<VECSIZE> 39<NULLD><MFCC_0_D_A><DIAGC>" > $WORK/hmm0/macros
else
  echo -n "~o<STREAMINFO> 1 12<VECSIZE> 12<NULLD><USER_E_D_A><DIAGC>" > $WORK/hmm0/macros
fi
cat $WORK/hmm0/vFloors >> $WORK/hmm0/macros

echo "Adding output mixture components..."
cat <<"EOF" > $WORK/$HHED_SCRIPT
MU 4 {*.state[2].mix} MU 4 {*.state[3].mix} MU 4 {*.state[4].mix} MU 4 {*.state[5].mix}
EOF

${HTK_PREFIX}HHEd  -H $WORK/hmm0/macros -H $WORK/hmm0/hmmdefs $WORK/$HHED_SCRIPT $WORK/$SYLLIST_COMPLETE

for iter in 0 1 2 3 4 5 6 7 8 9; do
  echo "Training iteration ${iter}..."
  let "nextiter=$iter+1"
  mkdir $WORK/hmm$nextiter
  ${HTK_PREFIX}HERest -C $WORK/$HMMCONFIG -I $WORK/$TRAIN_MLF \
                      -t 250.0 150.0 1000.0 -S $WORK/$TRAIN_SCRIPT \
                      -H $WORK/hmm$iter/macros -H $WORK/hmm$iter/hmmdefs \
                      -M $WORK/hmm$nextiter $WORK/$SYLLIST_COMPLETE
done

echo "Testing..."
for iter in 9; do
  ${HTK_PREFIX}HVite -H $WORK/hmm$iter/macros -H $WORK/hmm$iter/hmmdefs \
                     -C $WORK/$HMMCONFIG -S $WORK/$TEST_SCRIPT -i $WORK/$RECOUT \
                     -w $WORK/$WDNET -p 0.0 -s 5.0 $WORK/$DICT $WORK/$SYLLIST_COMPLETE
  echo "Results from testing on iteration ${iter}..."
  ${HTK_PREFIX}HResults -e "???" ${SILENCE} -I $WORK/$TEST_MLF $WORK/$SYLLIST_COMPLETE $WORK/$RECOUT
done

${HTK_PREFIX}HResults -p -t -e "???" ${SILENCE} \
             -I $WORK/$TEST_MLF $WORK/$SYLLIST_COMPLETE $WORK/$RECOUT > $WORK/$RESULTS_FILE
             
grep Aligned $WORK/$RESULTS_FILE | sed -E "s/.*\/..\/([a-z]{2})([0-9]{2,3}\.[0-9])p([0-9]{2,3}\.[0-9])s.*/\2 \3/" | sort | uniq -c > $WORK/$MISCLASSIFIED

echo "Final results, errors, and confusion matrix in file $WORK/$RESULTS_FILE"
echo "Statstics on misclassification in file $WORK/$MISCLASSIFIED"
echo "`wc -l $WORK/$SYLLIST` sounds in total"

if [ "$matlab_plot" ]
then
  echo "Plotting results figure in MATLAB..."
  cd matlab
  /Applications/MATLAB_R2007b/bin/matlab -nojvm -nosplash -r "plot_results('../${WORK}/'); exit"
  cd ..
fi