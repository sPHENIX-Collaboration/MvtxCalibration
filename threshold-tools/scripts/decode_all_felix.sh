#!/bin/bash

# Define your email configuration
run_numbers=("00038920" "00038921" "00038922"  "00038919" "00038923"  "00038924" )
date_of_scan="20240417"
type_of_scan="FakeHitRate"
threshold_scan_seconds=("130837" "130839" "130836" "130837" "130837" "130836")
for i in {0..5}
do
    
    RUNNUMBER=${run_numbers[i]}
    THRESHOLD_SCAN_DIR=/data/runs/${date_of_scan}_${threshold_scan_seconds[i]}_${type_of_scan}

    host=mvtx${i}.sphenix.bnl.gov
    ssh -T phnxrc@${host} << EOF
        cp -rf /data/runs/${date_of_scan}_${threshold_scan_seconds[i]}_${type_of_scan} /home/phnxrc/tmengel/FakeHitRates/${date_of_scan}_${threshold_scan_seconds[i]}_${type_of_scan}_mvtx${i}
        cd /home/phnxrc/tmengel/FakeHitRates/${date_of_scan}_${threshold_scan_seconds[i]}_${type_of_scan}_mvtx${i}
        /home/phnxrc/tmengel/scripts/decode-data.sh ${RUNNUMBER} /bbox/bbox${i}/MVTX/calib/ HotPix April_17_2024 > /home/phnxrc/tmengel/FakeHitRates/${date_of_scan}_${threshold_scan_seconds[i]}_${type_of_scan}_mvtx${i}/decode-run-${RUNNUMBER}.log 2>&1 &
EOF
done
