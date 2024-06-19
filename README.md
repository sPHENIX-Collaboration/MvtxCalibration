# MVTX Calibration

## Building the package

A package should be built before running the scripts, or remove any mention of these packages in the Fun4All macros so you can just process the data. 

There is an included script called `BuildAll.sh` which will build all the modules in `MvtxCalibration/src` and create condor files to submit jobs in `MvtxCalibration/condor`. This is an example of how to run the script:

```bash
export $MYINSTALL=<your install directory path>
source BuildAll.sh $MYINSTALL
```
You can also run `BuildAll.sh` without any arguments and it will default to installing in `MvtxCalibration/install`.
```bash
source BuildAll.sh
```

Help can be found by running `BuildAll.sh` with the `-h` flag.
```bash
source BuildAll.sh -h
```

### What does `BuildAll.sh` do?

The `BuildAll.sh` script will build the modules in `MvtxCalibration/src` and create condor files to submit jobs in `MvtxCalibration/condor`. It will also configure directories for output files and logs. A brief overview of what the script does is below.

1. Build the modules in `MvtxCalibration/src` and install them in the provided install directory.
	- `mvtxfhr` : Fake hit rate analysis module
	- `rawdatatools` : Tools to manage raw `.prdf` files

2. Create `MvtxCalibration/condor` directory with scripts to submit condor jobs of analysis on RCF.
	- `fhr_task.job`: Condor job script that calls exicutible and pipes in arguments from `args.list`.
	 
		- **Check that this file has the correct `initialDir`.**  For example mine is:
			
			`initialDir         = /sphenix/user/tmengel/MVTX/MvtxCalibration`
			
	- `run_fhr_task.job`: Exicutible file that sources install directory and calls `Fun4All_MvtxFHR.C`.

		- **Check that this file has the correct `$INSTALLDIR`.** It should be the path supplied to `BuildAll.sh`.

	- `args.list`: List file that contains arguments for each condor job.
		- The arguments are `<number of events>` `<run number>` `<trigger rate [kHz]>` `<output file path>` `<trigger guard output (optional could be "none")>`

		- **Check the output path for root files and make sure it leads to `MvtxCalibration/rootfiles`**

	- `MvtxCalibration/condor/log`: Directory for condor job logs and output files.

3. Create `MvtxCalibration/rootfiles` where output is saved from condor jobs.

After running `BuildAll.sh` your `MvtxCalibration` directory should look like this:
```bash
MvtxCalibration/
├── condor/
│   ├── args.list
│   ├── fhr_task.job
│   ├── log/
│   ├── run_fhr_task.job
├── rootfiles/
├── src/
│   ├── mvtxfhr/
│   ├── rawdatatools/
├── macros/
│   ├── Fun4All_MvtxFHR.C
├── README.md
├── BuildAll.sh
```
You could also have a `install` directory if you didn't provide an  an install directory to `BuildAll.sh`.


## What is in the package?

There are 2 modules in the `MvtxCalibration` package that are automatically build via the `BuildAll.sh` script. These modules are used to analyze raw MVTX data files and optimize the fake hit rate.

These modules are located in the `MvtxCalibration/src` directory. The modules are:

1. `mvtxfhr`
	- Contains classes and subsystem modules for doing a Fake hit rate analysis on raw `.prdf` files.
2. `rawdatatools`
	- Contains classes to find and manage raw `.prdf` files in the data directory on `RCF`.

### `mvtxfhr` module

This module contains classes and subsystem modules for doing a Fake hit rate analysis on raw `.prdf` files. The module contains classes for loading pixel masks from the conditions database, creating hitmaps from raw data, and calculating the fake hit rate. 

- `MvtxPixelDefs`
	- Contains definitions and methods for creating pixel keys for each pixel in the MVTX detector. The pixel key is a 64-bit integer that uniquely identifies each pixel in the detector.
- `MvtxPixelMask`
	- Class which loads pixel masks from the conditions database and provides methods for masking pixels in hitmaps.
- `MvtxHitMap`
	- Class which creates hitmaps from raw data files. Hitmap is a vector of pairs of pixel keys and the number of hits in that pixel.
- `MvtxFakeHitRate`
	- Main class for calculating the fake hit rate. Creates hitmaps from raw data files, masks pixels, and calculates the fake hit rate.
	- `SetOutputfile(std::string outputfile)`
		- Sets the output file for the fake hit rate analysis. The output file will contain the fake hit rate and the optimized pixel mask.
	- `SetMaxMaskedPixels(int max_masked_pixels)`
		- Sets the maximum number of pixels to mask in the optimization. The optimization will mask the `max_masked_pixels` pixels with the most hits in the hitmap.

- `MvtxTriggerRampGaurd`
	- Event selector that selects events based on change in BCO. This is used to remove strobes at the beginning of the run when the trigger rate is not stable.
	- `SaveOutput(std::string outputfile)`
		- Sets to debug mode. Saves histograms of $\Delta BCO$ both before and after the trigger ramp guard. Also saves TTree with all $\Delta BCO$ values.
	- `SetTriggerRate(double trigger_rate)`
		- Sets the trigger rate in kHz. This is used to calculate the expected number GTM periods between readout windows.
	- `SetConcurrentStrobeTarget(int target)`
		- Sets the target number of concurrent strobes that need to be less than the threshold to pass the trigger ramp guard.

### `rawdatatools` module

This module contains classes to find and manage raw `.prdf` files in the sPHENIX raw data directories on RCF. This allows users to find and load raw data files for analysis for any subsystem with a given run number.

- `RawDataDefs`
	- Contains definitions for the raw data file types and the directory structure of the raw data files on RCF. This also contains naming schemes for all subsystems.
	- Some useful definitions:
	  ```cpp
	  RawDataDefs::SPHNXPRO_COMM : "/sphenix/lustre01/sphnxpro/commissioning";
	  RawDataDefs::SPHNXPRO_PHYS : "/sphenix/lustre01/sphnxpro/physics";
	  RawDataDefs::BEAM : "beam";
	  RawDataDefs::CALIB : "calib";
	  RawDataDefs::COSMIC : "cosmic";
	  RawDataDefs::PHYSICS : "physics";
	  RawDataDefs::DATA : "data";
	  RawDataDefs::JUNK : "junk";
	  RawDataDefs::GL1 : "GL1";
	  RawDataDefs::HCAL : "HCal";
	  RawDataDefs::INTT : "INTT";
	  RawDataDefs::LL1 : "LL1";
	  RawDataDefs::MVTX : "MVTX";
	  RawDataDefs::TPOT : "TPOT";
	  RawDataDefs::ZDC : "ZDC";
	  RawDataDefs::EMCAL : "emcal";
	  RawDataDefs::MBD : "mbd";
	  RawDataDefs::TPC : "tpc";
	  ```
	 
- `RawFileUtils`
	- Namespace with utility function for determining if a raw prdf file is valid for analysis. This includes checking if the file is complete, and is the correct file type.
- `RawFileFinder`
	- Class for finding raw data files in the sPHENIX raw data directories on RCF. Uses bash commands to find files and directories based on run number and subsystem.
- `RawDataManager`
	- Main class for managing raw data files. This class finds raw data files, loads them into memory, and provides methods for accessing the raw data files.
	- Example usage:
		```cpp
		 	RawDataManager * rdm = new RawDataManager();
			rdm->SetDataPath(RawDataDefs::SPHNXPRO_COMM); // or RawDataDefs::SPHNXPRO_PHYS
			rdm->SetRunNumber(42639);
			rdm->SetRunType(RawDataDefs::CALIB); // physics, calib, junk, etc.
			rdm->SelectSubsystems({RawDataDefs::MVTX, RawDataDefs::GL1}); //
			rdm->GetAvailableSubsystems();
			rdm->Verbosity(0);
			std::vector<std::string> mvtx_files = rdm->GetFiles(
				RawDataDefs::MVTX,
				RawDataDefs::get_channel_name(RawDataDefs::MVTX, 2));
		```
		
		The above code will find all MVTX and GL1 files for the given run number and run type. The `iflx` variable is the file index, which is used to get the nth FELIX server for the MVTX. The `RawDataDefs::get_channel_name` function is used to get the channel name for the given subsystem and channel index. The `mvtx_files` vector will contain all the MVTX files for the given run number and run type.

		```cpp
		for (auto file : mvtx_files) {
			std::cout << file << std::endl;
		}
		```
		Output:
			
		```bash
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0000.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0001.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0002.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0003.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0004.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0005.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0006.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0007.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0008.evt
		/sphenix/lustre01/sphnxpro/commissioning/MVTX/calib/calib_mvtx2-00042639-0009.evt
		```

## Running the Fake Hit Rate Analysis

The fake hit rate analysis is run using the `Fun4All_MvtxFHR.C` macro in the `MvtxCalibration/macros` directory. This macro will run the fake hit rate analysis on raw MVTX data files and save the results to an output file. The macro takes the following arguments:
	
- `nEvents` : Number of events to process
- `run_number` : Run number of the data
- `trigger_rate_kHz` : Trigger rate in kHz
- `output_name` : Output file name
- `trigger_guard_output_name` : Output file name for trigger guard analysis (optional) 

The macro can be run using the following command:

```bash
root -l -b -q 'Fun4All_MvtxFHR.C(10000, 42641, 44, "output.root", "trigger_guard_output.root")'
```

The macro will initialze a Fun4All server,  then load raw MVTX data files for the given run number using the `RawDataManager` class. The macro will then create a `MvtxFakeHitRate` object and set the output file name and the maximum number of pixels to mask. The macro will then run the fake hit rate analysis and save the results to the output file. The output file will contain the fake hit rate and the optimized pixel mask.


