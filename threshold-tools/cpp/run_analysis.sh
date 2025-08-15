#!/bin/bash

# check if the number of arguments is correct
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <run_number>"
    exit 1
fi

RUNNUMBER=$1

# get the time stamp from the run number, only keep the date part and replace '-' with ''
TIMESTAMP=$(psql -tA -h sphnxdaqdbreplica daq --csv -c "SELECT brtimestamp FROM run WHERE runnumber=$RUNNUMBER" | cut -d' ' -f1 | tr -d '-')
echo "Timestamp for run $RUNNUMBER: $TIMESTAMP"

# create the results directory if it does not exist
OUTDIR=./results/${RUNNUMBER}_${TIMESTAMP}
echo "Output directory: $OUTDIR"
for i in {0..5}; do
    python thrana_plots.py --felix $i -d /sphenix/user/hjheng/sPHENIXRepo/MVTX/MvtxCalibration/threshold-tools/cpp/output/$RUNNUMBER/ -o $OUTDIR
done

root -l -q -b plotMvtxThreshold.C\(\"$OUTDIR\"\)

root -l -q -b MvtxThreshold_TimeSeries.C

# cd into the results directory and merge the pdf files
cd $OUTDIR
# merge the pdf files into one
gs -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE=mvtx_all.pdf -dBATCH mvtx*.pdf

cd ../

cp MvtxThreshold_TimeSeries*.p* /sphenix/WWW/subsystem/mvtx/Run2025/ThresholdScan/
cp MvtxRMS_TimeSeries*.p* /sphenix/WWW/subsystem/mvtx/Run2025/ThresholdScan/
cp -r ${RUNNUMBER}_${TIMESTAMP} /sphenix/WWW/subsystem/mvtx/Run2025/ThresholdScan/
cd /sphenix/WWW/subsystem/mvtx/Run2025/ThresholdScan/${RUNNUMBER}_${TIMESTAMP}
cp /sphenix/WWW/subsystem/mvtx/Run2025/index.php .
cp -r /sphenix/WWW/subsystem/mvtx/Run2025/res/ .

