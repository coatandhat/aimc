#!/bin/bash
# Script to run a single HMM train/test cycle with the given parameters.
# This script expects the following variables to be set
#
# total_hmm_states - total number of HMM states (including the 2 non-emitting states)
# mixture_components - number of components in the output distribution for each emitting state
# input_vector_size - number or elements in the input vector (normally 39 for MFCCs, 12 for AIM)
# feature_code - HTK feature type code for the features being used (normally MFCC_0_D_A for MFCCs and USER_E_D_A for AIM features)
# FEATURE_SUFFIXES - List of suffixes appended to the feature filenames. For the MFCCs this is just "mfc" but for the AIM feature, there can be multiple features generated from each run of AIMCopy
# WORK - working directory
# SYLLIST_COMPLETE

# Filenames generated here
HMMPROTO=hmm_prototype
HHED_SCRIPT=hhed_change_components_script
RECOUT=recognition_output
RESULTS_FILE=results
MISCLASSIFIED=misclassified_syllables

# Filenames used here
TRAIN_SCRIPT=training_script
TEST_SCRIPT=testing_script
SYLLIST_COMPLETE=syllable_list_with_silence

hmm_type=${total_hmm_states}states_${mixture_components}mixture_components
echo "HMM type: $hmm_type"
mkdir -p $WORK/$hmm_type

echo "Creating HMM structure..."
./gen_hmmproto.py --input_size ${input_vector_size} --total_hmm_states ${total_hmm_states} --feature_type ${feature_code} > $WORK/$hmm_type/$HMMPROTO

echo "Adding output mixture components..."
./gen_hhed_script.py --num_means ${mixture_components} --total_hmm_states ${total_hmm_states} > $WORK/$hmm_type/$HHED_SCRIPT


echo "Training HMM..."
echo "Setting up prototype HMM..."
mkdir -p $WORK/$hmm_type/hmm0
HCompV -C $WORK/$HMMCONFIG -f 0.01 -m -S $WORK/$TRAIN_SCRIPT -M $WORK/$hmm_type/hmm0 $WORK/$hmm_type/$HMMPROTO

echo "Generating HMM definitions..."
# Now take the prototype file from hmm0, and create the other HMM definitions
# from it
grep -A 9999 "<BEGINHMM>" $WORK/$hmm_type/hmm0/$HMMPROTO > $WORK/$hmm_type/hmm0/hmms
for syllable in $(cat $WORK/$SYLLIST_COMPLETE); do
  echo "~h $syllable" >> $WORK/$hmm_type/$feature/hmm0/hmmdefs
  cat $WORK/$hmm_type/$feature/hmm0/hmms >> $WORK/$hmm_type/$feature/hmm0/hmmdefs
done
 
echo -n "~o<STREAMINFO> 1 ${input_vector_size}<VECSIZE> ${input_vector_size}<NULLD><${feature_code}><DIAGC>" > $WORK/$hmm_type/$feature/hmm0/macros

cat $WORK/$hmm_type/$feature/hmm0/vFloors >> $WORK/$hmm_type/$feature/hmm0/macros

${HTK_PREFIX}HHEd  -H $WORK/$hmm_type/$feature/hmm0/macros -H $WORK/$hmm_type/$feature/hmm0/hmmdefs $WORK/$hmm_type/$HHED_SCRIPT $WORK/$SYLLIST_COMPLETE

for iter in $TRAINING_ITERATIONS_LIST; do
  echo "Training iteration ${iter}..."
  let "nextiter=$iter+1"
  mkdir $WORK/$hmm_type/hmm$nextiter
  ${HTK_PREFIX}HERest -C $WORK/$HMMCONFIG -I $WORK/$TRAIN_MLF \
    -t 250.0 150.0 1000.0 -S $WORK/$TRAIN_SCRIPT \
    -H $WORK/$hmm_type/hmm$iter/macros -H $WORK/$hmm_type/hmm$iter/hmmdefs \
    -M $WORK/$hmm_type/hmm$nextiter $WORK/$SYLLIST_COMPLETE
done

for iter in $TESTING_ITERATIONS_LIST; do
  echo "Testing iteration ${iter}..."
  ${HTK_PREFIX}HVite -H $WORK/$hmm_type/hmm$iter/macros -H $WORK/$hmm_type/hmm$iter/hmmdefs \
    -C $WORK/$HMMCONFIG -S $WORK/$TEST_SCRIPT -i $WORK/$hmm_type/$RECOUT \
    -w $WORK/$WDNET -p 0.0 -s 5.0 $WORK/$DICT $WORK/$SYLLIST_COMPLETE
  echo "Results from testing on iteration ${iter}..."
  ${HTK_PREFIX}HResults -e "???" ${SILENCE} -I $WORK/$TEST_MLF $WORK/$SYLLIST_COMPLETE $WORK/$hmm_type/$RECOUT
  ${HTK_PREFIX}HResults -p -t -e "???" ${SILENCE} \
    -I $WORK/$TEST_MLF $WORK/$SYLLIST_COMPLETE $WORK/$hmm_type/$RECOUT > $WORK/$hmm_type/${RESULTS_FILE}_iteration_$iter
  grep Aligned $WORK/$hmm_type/${RESULTS_FILE}_iteration_$iter| sed -E "s/.*\/..\/([a-z]{2})([0-9]{2,3}\.[0-9])p([0-9]{2,3}\.[0-9])s.*/\2 \3/" | sort | uniq -c > $WORK/$hmm_type/${MISCLASSIFIED}_iteration_$iter
done
