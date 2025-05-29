#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4allraw/InputManagerType.h>
#include <fun4allraw/SingleGl1PoolInput.h>
#include <fun4allraw/SingleMvtxPoolInput.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <ffarawmodules/StreamingCheck.h>

#include <phool/recoConsts.h>

#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>

#include <mvtx/MvtxRawPixelMasker.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libmvtx.so)

void Fun4All_MvtxHotPixelMask_Standalone(const int nEvents = 100000)
{

    // Initialize fun4all
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);
    const int run_number = 42641;

    // Input manager
    recoConsts *rc = recoConsts::instance();
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
    rc->set_uint64Flag("TIMESTAMP", run_number);
    rc->set_IntFlag("RUNNUMBER", run_number);

    Fun4AllStreamingInputManager *in = new Fun4AllStreamingInputManager("Comb");
    in->Verbosity(0);

    for(int iflx = 0; iflx < 1; iflx++)
    {
        SingleMvtxPoolInput  * mvtx_sngl = new SingleMvtxPoolInput("MVTX_FLX" + to_string(iflx));
        mvtx_sngl->SetBcoRange(100);
        std::string filelist = "mvtx" + to_string(iflx) + ".list";
        std::cout << "Adding files for flx " << iflx << std::endl;
        mvtx_sngl->AddListFile(filelist);
        in->registerStreamingInput(mvtx_sngl, InputManagerType::MVTX);
    }
    se->registerInputManager(in);


    SyncReco *sync = new SyncReco();
    se->registerSubsystem(sync);

    HeadReco *head = new HeadReco();
    se->registerSubsystem(head);

    FlagHandler *flag = new FlagHandler();
    se->registerSubsystem(flag);

    // MVTX Hot Pixel Masker
    MvtxRawPixelMasker *mmvt = new MvtxRawPixelMasker();
    mmvt->Verbosity(2);
    mmvt->set_threshold(1e-8);
    mmvt->set_update_every_nstrobe(1e3);
    mmvt->load_starting_from_CDB(true, "MVTX_HotPixelMap");
    se->registerSubsystem(mmvt);


    se->run(nEvents);
    se->End();
    delete se;
    gSystem->Exit(0);
    

}
