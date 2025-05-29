#!/bin/bash

# Define your email configuration
export MERGER=/bbox/bbox0/MVTX/thresholdscans/merger/concat-threshold-scans.sh
run_numbers=("00039793" "00039793" "00039793"  "00039793" "00039793"  "00035616" )
threshold_scan_dirs=("20240320_105707_ThresholdScan" "20240320_105709_ThresholdScan" "20240320_105707_ThresholdScan" "20240320_105706_ThresholdScan" "20240320_105707_ThresholdScan" "20240320_105707_ThresholdScan")
for i in {0..0}
do
    
    RUNNUMBER=${run_numbers[i]}
    THRESHOLD_SCAN_DIR=${threshold_scan_dirs[i]}

    host=mvtx${i}.sphenix.bnl.gov
    ssh -T phnxrc@${host} << EOF
        cd /bbox/bbox0/MVTX/thresholdscans/merger
        ./concat-threshold-scans.sh ${i} ${RUNNUMBER} ${THRESHOLD_SCAN_DIR} > /bbox/bbox0/MVTX/thresholdscans/mvtx${i}/merge-run-${RUNNUMBER}.log 2>&1 &
EOF
done
