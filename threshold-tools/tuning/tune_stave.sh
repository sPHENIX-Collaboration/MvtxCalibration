#!/bin/bash


source /opt/sphenix/core/bin/sphenix_setup.sh -n new
export MYINSTALL=/sphenix/user/tmengel/MVTX/threshold-tuning/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL


script=$1
dir=$2
flx=$3
file=$4
run_base=$5
tuner=$6

echo "Running $script $dir $flx $file"
echo "Run base: $run_base"

python3 $script $dir $flx $file 

output_dir=$dir/../ThrsTuneResults 
if [ -d $output_dir ]; then
    rm -rf $output_dir
fi
if [ ! -d $output_dir ]; then
    mkdir -p $output_dir
fi

for electrons in {10..13};
do 
    python3 $tuner $dir $electrons
    png_file=$dir/threshold_tuning${electrons}0e.png
    json_file=$dir/threshold_tuned${electrons}0e.json
    stave_dir=$output_dir/${flx}
    if [ ! -d $stave_dir ]; then
        mkdir -p $stave_dir
    fi
    cp $png_file $stave_dir/${flx}_threshold_tuning${electrons}0e.png
    cp $json_file $stave_dir/${flx}_threshold_tuned${electrons}0e.json
done
cp $dir/run_parameters.json $output_dir/${flx}_run_parameters.json

