# MVTX Calibration

## Building the analysis modules

A package should be built before running the scripts, or remove any mention of these packages in the Fun4All macros so you can just process the data. There is an included script called `BuildAll.sh` which will build all the modules in `MvtxCalibration/src` and create condor files to submit jobs in `MvtxCalibration/condor`.
```bash
export $MYINSTALL=<your install directory path>
source BuildAll.sh $MYINSTALL
```
This will build `MvtxCalibration/src/mvtxfhr` and `MvtxCalibration/src/rawdatatools` and install them in your install directory. It should also create two files in `MvtxCalibration/condor`. 

1. `fhr_task.job`
   - Check that this file has the correct `initialDir`. For example mine is:
  	```bash
	initialDir         = /sphenix/user/tmengel/MVTX/MvtxCalibration
	```
2. `run_fhr_task.sh`
   - Check this file as well for the correct install path and macro directory.

The `BuildAll.sh` should also create a sub-directory within your condor directory called `MvtxCalibration/condor/log`. This is where the .err and .out files from the condor job will live after you submit it.

## What is in the package

There are 2 modules in the `MvtxCalibration` package that are automatically build via the `BuildAll.sh` script.

The modules are:

1. `mvtxfhr`
	- Contains classes and subsystem modules for doing a Fake hit rate analysis on raw `.prdf` files.
2. `rawdatatools`
	- Contains classes to find and manage raw `.prdf` files in the data directory on `RCF`.

### Fun4All_MvtxFHR.C

This is the macro that runs the fake hit rate. It will take raw MVTX evt (PRDF) files, combine them into single Fun4All events, convert them into hitmaps then mask the nth pixels with the most hits. It will save the results of these optimizations to an output file `output_<runnumber>.root`.
