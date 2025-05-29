#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <GlobalVariables.C>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4allraw/InputManagerType.h>
#include <fun4allraw/SingleMvtxPoolInput.h>

#include <phool/recoConsts.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/SyncReco.h>

#include <mvtx/MvtxCombinedRawDataDecoder.h>
#include <mvtx/MvtxRawPixelMasker.h>
#include <mvtxdebug/MvtxHotPixelMaskWriter.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libmvtx.so)
R__LOAD_LIBRARY(libmvtxdebug.so)

#endif

namespace Enable
{
  bool MVTX_TRIGGERED = false;
}

namespace MVTX_MASKING 
{
  bool enable_offline_masking = false;
  float threshold = 1e-8; // 1e-6 , 1e-9
  int update_every_nstrobe = 1e6; // 1e4, ...
  bool load_starting_from_CDB = true;
  std::string hot_pixel_map = "MVTX_HotPixelMap";
  bool enable_dynamic = false;
}

void Mvtx_HitUnpacking(const std::string& felix="")
{

  int verbosity = std::max(Enable::VERBOSITY, Enable::MVTX_VERBOSITY);
  Fun4AllServer* se = Fun4AllServer::instance();
  
  if ( MVTX_MASKING::enable_offline_masking ) {
    
    std::cout << "Mvtx_HitUnpacking: Offline masking enabled" << std::endl;

    auto mvtxmasker = new MvtxRawPixelMasker();
    mvtxmasker -> set_threshold( MVTX_MASKING::threshold );
    mvtxmasker -> set_update_every_nstrobe( MVTX_MASKING::update_every_nstrobe );
    mvtxmasker -> load_starting_from_CDB( MVTX_MASKING::load_starting_from_CDB, 
                                      MVTX_MASKING::hot_pixel_map );
    mvtxmasker -> enable_dynamic_masking( MVTX_MASKING::enable_dynamic );
    mvtxmasker -> Verbosity(verbosity);

    se -> registerSubsystem(mvtxmasker);
  }

  auto mvtxunpacker = new MvtxCombinedRawDataDecoder();
  mvtxunpacker -> Verbosity(verbosity);
  std::cout << "MvtxCombineRawDataDecoder: run triggered mode? " << Enable::MVTX_TRIGGERED << std::endl;
  mvtxunpacker -> runMvtxTriggered(Enable::MVTX_TRIGGERED);
  mvtxunpacker -> doOfflineMasking(MVTX_MASKING::enable_offline_masking);
  if(felix.length() > 0) {
    mvtxunpacker -> useRawHitNodeName("MVTXRAWHIT_" + felix);
    mvtxunpacker -> useRawEvtHeaderNodeName("MVTXRAWEVTHEADER_" + felix);
  }
  se -> registerSubsystem(mvtxunpacker);
}

int Fun4All_MvtxMasker_Test()
{

  bool doDebug = true;
  int nEvents = 100000;
  const int run_number = 42641;
  const std::string & input_file00 = "mvtx0.list";
  const std::string & input_file01 = "mvtx1.list";
  const std::string & input_file02 = "mvtx2.list";
  const std::string & input_file03 = "mvtx3.list";
  const std::string & input_file04 = "mvtx4.list";
  const std::string & input_file05 = "mvtx5.list";
  
  const std::string & outputfile_debug = "mvtx_raw_hits_debug.root";
  const std::string & outputfile = "mvtx_raw_hits.root";

  std::vector< std::string > infiles {};
 
  infiles.push_back( input_file00 );
  if( false ) {
    infiles.push_back( input_file01 );
    infiles.push_back( input_file02 );
    infiles.push_back( input_file03 );
    infiles.push_back( input_file04 );
    infiles.push_back( input_file05 );
  }

  Enable::VERBOSITY = 1;
  Enable::MVTX_VERBOSITY = 1;
  Enable::MVTX_TRIGGERED = false;

  MVTX_MASKING::enable_offline_masking = true;
  MVTX_MASKING::threshold = 1e-8;
  MVTX_MASKING::update_every_nstrobe = 1e4;
  MVTX_MASKING::load_starting_from_CDB = true;
  MVTX_MASKING::hot_pixel_map = "MVTX_HotPixelMap";
  MVTX_MASKING::enable_dynamic = true;

  auto se = Fun4AllServer::instance();
  se -> Verbosity( Enable::VERBOSITY );

  auto rc = recoConsts::instance();
  Enable::CDB = true;
  rc->set_StringFlag( "CDB_GLOBALTAG", CDB::global_tag );
  rc->set_uint64Flag( "TIMESTAMP", run_number );
  rc->set_IntFlag( "RUNNUMBER", run_number );

  int iflx = 0;
  auto in = new Fun4AllStreamingInputManager( "Comb" );
  for (auto  & infile : infiles ) {
    auto sngl= new SingleMvtxPoolInput( "MVTX_FLX" + to_string(iflx) );
    sngl -> SetBcoRange( 100 );
    sngl -> AddListFile( infile );
    in -> registerStreamingInput( sngl, InputManagerType::MVTX );
    iflx++;
  }
  se -> registerInputManager( in );

  Mvtx_HitUnpacking();

  if ( doDebug ) {
    auto out_debug = new MvtxHotPixelMaskWriter();
    out_debug -> set_output_filename( outputfile_debug );
    se -> registerSubsystem( out_debug );
  }
 

  auto out = new Fun4AllDstOutputManager( "out", outputfile );
  se -> registerOutputManager( out );

  auto sync = new SyncReco();
  se -> registerSubsystem( sync );

  auto head = new HeadReco();
  se -> registerSubsystem( head );

  auto flag = new FlagHandler();
  se -> registerSubsystem( flag );

  se -> run( nEvents );
  se -> End();

  CDBInterface::instance() -> Print();
  se -> PrintTimer();

  delete se;
  gSystem -> Exit(0);

  return 0;

}