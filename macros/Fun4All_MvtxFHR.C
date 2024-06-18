#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4allraw/InputManagerType.h>
#include <fun4allraw/SingleGl1PoolInput.h>
#include <fun4allraw/SingleMvtxPoolInput.h>

#include <phool/recoConsts.h>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>

#include <ffarawmodules/StreamingCheck.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <rawdatatools/RawDataManager.h>
#include <rawdatatools/RawDataDefs.h>
#include <rawdatatools/RawFileUtils.h>

#include <mvtxfhr/MvtxTriggerRampGaurd.h>
#include <mvtxfhr/MvtxFakeHitRate.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libffarawmodules.so)

R__LOAD_LIBRARY(librawdatatools.so)
R__LOAD_LIBRARY(libMvtxFHR.so)

void Fun4All_MvtxFHR(const int nEvents = -1,
                    const int run_number = 42641
                        )
{  
    
    const std::string output_name = "fhr_calib"+std::to_string(run_number)+".root";
    const std::string run_type="calib";

    // raw data manager
    RawDataManager * rdm = new RawDataManager();
    rdm->SetDataPath(RawDataDefs::SPHNXPRO_COMM);
    rdm->SetRunNumber(run_number);
    rdm->SetRunType(run_type);
    rdm->SelectSubsystems({RawDataDefs::MVTX});
    rdm->GetAvailableSubsystems();
    rdm->Verbosity(0);

    // Initialize fun4all
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);

    // Input manager
    recoConsts *rc = recoConsts::instance();
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
    rc->set_uint64Flag("TIMESTAMP", run_number);
    rc->set_IntFlag("RUNNUMBER", run_number);

    Fun4AllStreamingInputManager *in = new Fun4AllStreamingInputManager("Comb");
    in->Verbosity(0);

    for(int iflx = 0; iflx < 6; iflx++)
    {
        SingleMvtxPoolInput  * mvtx_sngl = new SingleMvtxPoolInput("MVTX_FLX" + to_string(iflx));
        mvtx_sngl->SetBcoRange(10);
        std::vector<std::string> mvtx_files = rdm->GetFiles(RawDataDefs::MVTX, RawDataDefs::get_channel_name(RawDataDefs::MVTX, iflx));
        std::cout << "Adding files for flx " << iflx << std::endl;
        for (const auto &infile : mvtx_files)
        {
            if(!RawFileUtils::isGood(infile)){  continue; }
            std::cout << "\tAdding file: " << infile << std::endl;
            mvtx_sngl->AddFile(infile);
        }
        in->registerStreamingInput(mvtx_sngl, InputManagerType::MVTX);
    }
    se->registerInputManager(in);


    SyncReco *sync = new SyncReco();
    se->registerSubsystem(sync);

    HeadReco *head = new HeadReco();
    se->registerSubsystem(head);

    FlagHandler *flag = new FlagHandler();
    se->registerSubsystem(flag);

    MvtxTriggerRampGaurd *mgc = new MvtxTriggerRampGaurd();
    mgc->SetTriggerRate(44000.0);
    se->registerSubsystem(mgc);

    MvtxFakeHitRate *mfhr = new MvtxFakeHitRate();
    mfhr->SetOutputfile(output_name);
    mfhr->SetMaxMaskedPixels(100000);
    se->registerSubsystem(mfhr);

    se->run(nEvents);
    se->End();
    delete se;
    gSystem->Exit(0);
    

}
