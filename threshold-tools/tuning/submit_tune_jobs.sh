#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
export INSTALLDIR=$PWD/install
if [ ! -d $INSTALLDIR ]; then
    mkdir -p $INSTALLDIR
fi
source /opt/sphenix/core/bin/setup_local.sh $INSTALLDIR
echo $INSTALLDIR


# Get the directories for the threshold tuning
current_dir=$(pwd)
dir_base=/sphenix/user/tmengel/MVTX/MVTX/calib_runs
mvtx_thrana=/sphenix/user/tmengel/MVTX/felix-mvtx/software/py/ib_tools/analysis/analysethrtuning_mvtx.py
mvtx_tuner=/sphenix/user/tmengel/MVTX/felix-mvtx/software/py/ib_tools/analysis/tuneMVTX.py

for run_base in 20240411_2006 20240411_1939; do

    output_dir=$current_dir/ThrsTune_${run_base}
    # remove the output directory if it exists
    if [ -d $output_dir ]; then
        rm -rf $output_dir
    fi
    if [ ! -d $output_dir ]; then
        mkdir -p $output_dir
    fi

    # arg list
    arglist=$current_dir/thrs_args_${run_base}.list
    if [ -f $arglist ]; then
        rm -f $arglist
    fi

    for i in {0..5}
    do 
        run_dir=$dir_base/mvtx$i
        cd $run_dir
        # get the run directories
        run_dirs=$(ls -d $run_base*)
        run_dirs=$(echo $run_dirs | tr " " "\n")

        run_number=$(echo $run_dirs | cut -d'_' -f 4)

        #remove 'run' from the run number
        run_number=$(echo $run_number | cut -d'r' -f 2) 
        run_number=$(echo $run_number | cut -d'u' -f 2)
        run_number=$(echo $run_number | cut -d'n' -f 2)  

        cp -rf $run_dirs $output_dir/mvtx${i}_runparams
        stave_dir=$output_dir/mvtx${i}_runparams
        cd $stave_dir

        prdf_file=/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx${i}-${run_number}-0000.evt
        echo "${mvtx_thrana}, ${stave_dir}, FLX${i}, ${prdf_file}, ${run_base}, ${mvtx_tuner}" >> $arglist
    done

    condor_job_file_name=$current_dir/ThrsTune_${run_base}.job
    if [ -f $condor_job_file_name ]; then
        rm -f $condor_job_file_name
    fi
    condor_log_dir=$current_dir/logs
    if [ ! -d $condor_log_dir ]; then
        mkdir -p $condor_log_dir
    fi

    echo "Universe           = vanilla" >> $condor_job_file_name
    echo "initialDir         =$current_dir" >> $condor_job_file_name
    echo "Executable         = \$(initialDir)/tune_stave.sh" >> $condor_job_file_name
    echo "PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)" >> $condor_job_file_name
    echo "request_memory     = 8GB" >> $condor_job_file_name
    echo "Priority           = 20" >> $condor_job_file_name
    echo "condorDir          = \$(initialDir)" >> $condor_job_file_name
    echo "Output             = \$(condorDir)/logs/condor-\$(Flx)-\$(RunBase).out" >> $condor_job_file_name
    echo "Error              = \$(condorDir)/logs/condor-\$(Flx)-\$(RunBase).err" >> $condor_job_file_name
    echo "Log                = /tmp/condor-\$(Flx)-\$(RunBase).log" >> $condor_job_file_name
    echo "Arguments          = \$(Script) \$(Dir) \$(Flx) \$(File) \$(RunBase) \$(Tuner)" >> $condor_job_file_name
    echo "Queue Script, Dir, Flx, File, RunBase, Tuner from $arglist" >> $condor_job_file_name

    condor_submit $condor_job_file_name
done






