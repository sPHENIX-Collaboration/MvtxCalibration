#!/bin/bash

# first argument is install directory
if [ $# -ne 1 ]; then
  echo "Usage: $0 <install directory>"
  exit 1
fi

# if first argument is -h or --help, print help
if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
  echo "Usage: $0 <install directory>"
  exit 0
fi

echo "Installing MvtxCalibration in $1"
export INSTALLDIR=$1
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $INSTALLDIR

CURRENTDIR=$PWD
SRCDIR=$PWD/src
#get list of subdirectories
SUBDIRS=$(find $SRCDIR -mindepth 1 -maxdepth 1 -type d)
for SUBDIR in $SUBDIRS
do
  echo "Building $SUBDIR"
  cd $SUBDIR
  # make build directory
  mkdir -p build
  cd build
  # execute autogen.sh
  $SUBDIR/autogen.sh --prefix=$INSTALLDIR

  make -j4
  make install
done

source /opt/sphenix/core/bin/setup_local.sh $INSTALLDIR
echo "MvtxCalibration installed in $INSTALLDIR"


# create condor scripts
echo "Creating condor scripts"
cd $CURRENTDIR/condor
# see if condor job file exists
if [ -f "fhr_task.job" ]; then
  echo "fhr_task.job already exists"
  rm -f fhr_task.sh
fi 
echo "Creating fhr_task.job"
echo "Universe           = vanilla" > fhr_task.job
echo "initialDir         = ${CURRENTDIR}/condor" >> fhr_task.job
echo "Executable         = \$(initialDir)/run_fhr_task.sh" >> fhr_task.job
echo "PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)" >> fhr_task.job
echo "request_memory     = 6GB" >> fhr_task.job
echo "Priority           = 20" >> fhr_task.job
echo "Output             = \$(initialDir)/log/condor-mvtx-fhr-\$INT(Process,%05d).out" >> fhr_task.job
echo "Error              = \$(initialDir)/log/condor-mvtx-fhr-\$INT(Process,%05d).err" >> fhr_task.job
echo "Log                = /tmp/condor-mvtx-fhr-\$ENV(USER)-\$INT(Process).log" >> fhr_task.job
echo "Queue 1" >> fhr_task.job

# see if run_fhr_task.sh exists
if [ -f "run_fhr_task.sh" ]; then
    echo "run_fhr_task.sh already exists"
    rm -f run_fhr_task.sh
fi 

echo "#!/bin/bash" > run_fhr_task.sh
echo "" >> run_fhr_task.sh
echo "source /opt/sphenix/core/bin/sphenix_setup.sh -n new" >> run_fhr_task.sh
echo "export INSTALLDIR=${INSTALLDIR}" >> run_fhr_task.sh
echo "source /opt/sphenix/core/bin/setup_local.sh \$INSTALLDIR" >> run_fhr_task.sh
echo "" >> run_fhr_task.sh
echo "cd ${CURRENTDIR}/macros" >> run_fhr_task.sh
echo "root -l -q -b Fun4All_MvtxFHR.C" >> run_fhr_task.sh
echo "echo \"Script done\"" >> run_fhr_task.sh

# make run_fhr_task.sh executable
chmod +x run_fhr_task.sh

echo "Condor scripts created"
echo "Done"
cd $CURRENTDIR
