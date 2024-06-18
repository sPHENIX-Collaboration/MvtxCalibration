#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
export INSTALLDIR=/sphenix/u/tmengel/installDir
source /opt/sphenix/core/bin/setup_local.sh $INSTALLDIR

cd /sphenix/user/tmengel/MVTX/MvtxCalibration/macros
root -l -q -b Fun4All_MvtxFHR.C
echo "Script done"
