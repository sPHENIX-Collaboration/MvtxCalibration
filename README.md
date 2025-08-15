# MVTX Calibration

## [New] Run2025 fast threshold scan
The main analysis code for the Run2025 fast threshold scan is located in:
```
threshold-tools/cpp
```

0. Make sure that the raw threshold scan data files are available at SDCC. For example, run 72324 has the following files:
	```
	/sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx0-00072324-0000.evt
	/sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx1-00072324-0000.evt
	/sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx2-00072324-0000.evt
	/sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx3-00072324-0000.evt
	/sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx4-00072324-0000.evt
	/sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx5-00072324-0000.evt
	```
	You can check the files for other run numbers in the same directory [1].

1. Compile the codes 
	```bash
	cd threshold-tools/cpp
	make
	```
	This will generate two executables: `thrsana` and `file-merger`. Only `thrsana` is needed for the threshold scan analysis.  
	- In `run_decoder_condor.sh`, change 
		```bash
		export MYINSTALL=/sphenix/u/hjheng/install
		```
		to your install directory
2. Run and submit the analysis jobs (via condor)
	- Navigate to the `condor` directory, you should see a python script `createfilelist.py` which creates/overwrites the condor argument file `queue.list` with the run number to be analyzed 
	- In `condor.job`, change 
		```bash
		Initialdir         = /sphenix/user/hjheng/sPHENIXRepo/MVTX/MvtxCalibration/threshold-tools/cpp
		```
		to your working directory.
	- Change the run number to analyze in `createfilelist.py` by the following command
		```bash
		sed -i 's/^runs *= *\[[0-9]\+\]/runs = [RUN_NUMBER]/' createfilelist.py # replace RUN_NUMBER to the desired run number
		```
	- Run `createfilelist.py` to generate or update the `queue.list` file
		```bash
		python createfilelist.py
		```
	- Confirm that `queue.list` has the correct lines for the run number. For example, if run 72324 is to be analyzed, `queue.list` should contain the line:
		```
		2001 /sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx0-00072324-0000.evt 3 Calib_00072324_0000_FEEID3
		2001 /sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx0-00072324-0000.evt 4 Calib_00072324_0000_FEEID4
		2001 /sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx0-00072324-0000.evt 259 Calib_00072324_0000_FEEID259
		2001 /sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx0-00072324-0000.evt 260 Calib_00072324_0000_FEEID260
		2001 /sphenix/lustre01/sphnxpro/physics/MVTX/calib/calib_mvtx0-00072324-0000.evt 515 Calib_00072324_0000_FEEID515
		```
	- submit the condor jobs by
		```bash
		condor_submit condor.job
		```
		A total of 144 jobs should be submitted.
	- Once the condor jobs are complete, you can find the output .dat files under the `threshold-tools/cpp/output/[RUN_NUMBER]` directory. Each FEE ID has 2 .dat files, `Calib_000[RUN_NUMBER]_0000_FEEID[FEE_ID]_thrs.dat` and `Calib_000[RUN_NUMBER]_0000_FEEID[FEE_ID]_rmss.dat`

3. Make analysis plots. under `threshold-tools/cpp`, 3 scripts/macros are used:
	- `thrana_plots.py`: this script takes .dat files, produces threshold/RMS plots for each pulsed pixel, computes average threshold/RMS per chip and stave
	- `plotMvtxThresholds.C`: plots stave average threshold and RMS as 2D heatmaps
	- `MvtxThreshold_TimeSeries.C`: generates time series plots for threshold/RMS across runs. Skip runs using hardcoded filters like:
		```
		// conditions to skip
		if ((run_number == "67472" && (mvtx_id == 3)) // skip mvtx_id 3 for run 67472
		...
		|| (run_number == "71526") // skip run 71526 fully (since we have run 71528)
		|| (run_number == "71527") // skip run 71527 fully (since we have run 71528)
		|| (run_number == "71790") // skip run 71528 fully (since we have run 71529)
		|| (run_number == "71792") // skip run 71792 fully (since we have run 71793)
		|| (run_number == "71793") // skip run 71793 fully
		|| (run_number == "71877") // skip run 71877 fully
		|| (run_number == "71878") // skip run 71878 fully
		|| (run_number == "71879") // skip run 71879 fully (since we have run 71880)
		|| (run_number == "72011") // skip run 72011 fully
		|| (run_number == "72012") // skip run 72012 fully
		|| (run_number == "72013") // skip run 72013 fully
		|| (run_number == "72014") // skip run 72014 fully
		|| (run_number == "72015") // skip run 72015 fully
		|| (run_number == "72321") // skip run 72321 fully
		|| (run_number == "72324") // skip run 72324 fully
		)
		{
			continue;
		}
		```

	A bash script `run_analysis.sh` does everything in one shot
	```bash
	./run_analysis.sh [RUN_NUMBER]
	```
	The result plots will be produced under `threshold-tools/cpp/results`. The script also sends the results to the webpage https://sphenix-intra.sdcc.bnl.gov/WWW/subsystem/mvtx/Run2025/ThresholdScan/

For issues or questions regarding the threshold scan analysis, please contact Hao-Ren Jheng (hrjheng@mit.edu, or via Mattermost). 

[1] If needed, I have a handy bash script that locates PRDF files by run number, subsystem, and run type. Reach out if you’re interested.

---

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
- `selected_layer` : Selected layer to analyze (optional, -1 for all layers)
- `trigger_guard_output_name` : Output file name for trigger guard analysis (optional) 

The macro can be run using the following command:

```bash
root -l -b -q 'Fun4All_MvtxFHR.C(10000, 42641, 44, "output.root", "-1", "trigger_guard_output.root")'
```

The macro will initialze a Fun4All server,  then load raw MVTX data files for the given run number using the `RawDataManager` class. The macro will then create a `MvtxFakeHitRate` object and set the output file name and the maximum number of pixels to mask. The macro will then run the fake hit rate analysis and save the results to the output file. The output file will contain the fake hit rate and the optimized pixel mask.

## Generating Pixel Masks

To generate a json file with the pixel mask, you can use the `MakeJsonHotPixelMap.C` macro in the `MvtxCalibration/macros` directory. This macro will read the output file from the fake hit rate analysis and generate a json file with the pixel mask. The macro takes the following arguments:

- `calibration_file` : Output file from the fake hit rate analysis
- `target_threshold` : Target threshold for fake hit rate

The macro can be run using the following command:

```bash
root -l -b -q 'MakeJsonHotPixelMap.C("output.root", 10e-8)'
```

The macro will read the output file from the fake hit rate analysis and generate a json file with the pixel mask. The json file will contain the pixel mask and the fake hit rate for each pixel. The macro will also print the number of pixels masked and the fake hit rate threshold.


## Questions

This is a work in progress and likely has some bugs. If you have any questions or need help, please contact me at
` <tmengel@vols.utk.edu>`.
```



