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
  RawDataDefs.h \
  RawFileFinder.h \
  RawDataManager.h \  
  RawFileUtils.h

lib_LTLIBRARIES = \
  librawdatatools.la

librawdatatools_la_SOURCES = \
  RawDataDefs.cc \
  RawFileFinder.cc \
  RawDataManager.cc \
  RawFileUtils.cc

librawdatatools_la_LIBADD = \
  -lphool 

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = librawdatatools.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
