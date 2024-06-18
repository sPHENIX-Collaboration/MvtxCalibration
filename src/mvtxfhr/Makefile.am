AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  MvtxFakeHitRate.h \
  MvtxHitMap.h \
  MvtxPixelDefs.h \
  MvtxPixelMask.h \ 
  MvtxTriggerRampGaurd.h
  
lib_LTLIBRARIES = \
  libMvtxFHR.la

libMvtxFHR_la_SOURCES = \
  MvtxFakeHitRate.cc \
  MvtxHitMap.cc \
  MvtxPixelDefs.cc \
  MvtxPixelMask.cc \
  MvtxTriggerRampGaurd.cc

libMvtxFHR_la_LIBADD = \
  -lphool \
  -ltrack_io \
  -lg4detectors \
  -ltrackbase_historic_io \
  -lmvtx \
  -lffarawobjects \
  -lcdbobjects \
  -lCLHEP \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libMvtxFHR.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)

