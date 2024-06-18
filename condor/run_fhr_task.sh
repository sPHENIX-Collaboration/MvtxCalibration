#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
export INSTALLDIR=/sphenix/user/tmengel/MVTX/mvtx-fhr/install
source /opt/sphenix/core/bin/setup_local.sh $INSTALLDIR

cd /sphenix/user/tmengel/MVTX/mvtx-fhr/macros
root -l -q -b Fun4All_MvtxFHR.C
echo "Script done"
