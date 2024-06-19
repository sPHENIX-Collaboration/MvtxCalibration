# MVTX Calibration

## Building the analysis

A package should be built before running the scripts, or remove any mention of these packages in the Fun4All macros so you can just process the data. 

There is an included script called `BuildAll.sh` which will build all the modules in `MvtxCalibration/src` and create condor files to submit jobs in `MvtxCalibration/condor`.
```bash
export $MYINSTALL=<your install directory path>
source BuildAll.sh $MYINSTALL
```

If no path for install directory is supplied, script will default to installing in `MvtxCalibration/install`. 

The `BuildAll.sh` will do the following things:
1. Make and install `mvtxfhr` and `rawdatatools` in provided install directory (default is `MvtxCalibration/install`.

2. Create `MvtxCalibration/condor` directory with scripts to submit condor jobs of analysis on RCF
	- `fhr_task.job`: Condor job script that calls exicutible and pipes in arguments from `args.list`.

   		**Check that this file has the correct `initialDir`.** For example mine is:
     			```bash
				initialDir         = /sphenix/user/tmengel/MVTX/MvtxCalibration
			```
 
 	- `run_fhr_task.job`: Exicutible file that sources install directory and calls `Fun4All_MvtxFHR.C`.

    		**Check that this file has the correct `$INSTALLDIR`.** It should be the path supplied to `BuildAll.sh`.
	
 	- `args.list`: List file that contains arguments for each condor job. The arguments are

    		`<number of events>` `<run number>` `<trigger rate [kHz]>` `<output file path>` `<trigger guard output (optional could be "none")>`

    		** Check the output path for root files and make sure it leads to `MvtxCalibration/rootfiles`**
	
 	- The `BuildAll.sh` should also create a sub-directory within your condor directory called `MvtxCalibration/condor/log`. This is where the .err and .out files from the condor job will live after you submit it.

3. Create `MvtxCalibration/rootfiles` where output is saved from condor jobs.
   

## What is in the package

There are 2 modules in the `MvtxCalibration` package that are automatically build via the `BuildAll.sh` script.

The modules are:

1. `mvtxfhr`
	- Contains classes and subsystem modules for doing a Fake hit rate analysis on raw `.prdf` files.
2. `rawdatatools`
	- Contains classes to find and manage raw `.prdf` files in the data directory on `RCF`.

### Fun4All_MvtxFHR.C

This is the macro that runs the fake hit rate. It will take raw MVTX evt (PRDF) files, combine them into single Fun4All events, convert them into hitmaps then mask the nth pixels with the most hits. It will save the results of these optimizations to an output file `output_<runnumber>.root`.
