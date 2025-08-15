#!/bin/bash

FILENAME=/sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx0-00067472-0000.evt

PREFIX=Calib_00067472_0000_FEEID
N=1000

for FEEID in 3 4 259 260 515 516 8193 8194 8449 8450 8705 8706; do
   echo "Running $FILENAME for FEEID $FEEID"
   ddump -s -g -n $N -p 2001 $FILENAME | thrsana -f $FEEID -i 25 -n 5 -p $PREFIX$FEEID -t 1
done

for FEEID in 5 261 517 8195 8196 8207 8451 8452 8463 8707 8708 8719; do
   echo "Running $FILENAME for FEEID $FEEID"
   ddump -s -g -n $N -p 2002 $FILENAME | thrsana -f $FEEID -i 25 -n 5 -p $PREFIX$FEEID -t 1
done