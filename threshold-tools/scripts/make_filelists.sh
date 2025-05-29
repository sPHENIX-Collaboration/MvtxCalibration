#!/bin/bash

export DATADIR=/bbox/bbox0/MVTX/thresholdscans/mvtx
runnumbers=("00033538" "00033537" "00033538" "00033537" "00033536" "00033536")

for i in {0..5} 
do 
    RUNNUMBER=${runnumbers[i]}
    FELIX=$i
    FILELIST=$PWD/filelists/mvtx${FELIX}_run${RUNNUMBER}.list
    INPUTFILES=$(ls $DATADIR${FELIX}/*${RUNNUMBER}*.evt)
    for FILE in $INPUTFILES
    do
        echo $FILE >> $FILELIST
    done
done