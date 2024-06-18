#ifndef MVTXFHR_MVTXFAKEHITRATE_H
#define MVTXFHR_MVTXFAKEHITRATE_H

#include "MvtxPixelMask.h"
#include "MvtxPixelDefs.h"
#include "MvtxHitMap.h"

#include <fun4all/SubsysReco.h>

#include <ffarawobjects/MvtxRawEvtHeader.h>
#include <ffarawobjects/MvtxRawHitContainer.h>

#include <TTree.h>
#include <TH1D.h>

#include <cstdint>
#include <iostream>
#include <string>

class PHCompositeNode;

class MvtxFakeHitRate : public SubsysReco
{
    public:

        MvtxFakeHitRate(const std::string& name = "MvtxFakeHitRate") : SubsysReco(name) {}
        ~MvtxFakeHitRate() override;

        // standard Fun4All functions
        int InitRun(PHCompositeNode*) override;
        int process_event(PHCompositeNode*) override;
        int End(PHCompositeNode*) override;
        
        void SetOutputfile(const std::string& name) { m_outputfile = name;}
        void SetMaxMaskedPixels(int n) { m_max_masked_pixels = n; }
        

    private:

        // optional output
        std::string m_outputfile{"mvtx_fhr.root"};

        int m_max_masked_pixels{1000};
        bool m_masked_pixels_in_file{false};
        uint64_t m_last_strobe{0};
        // mask hot pixels
        MvtxPixelMask * m_hot_pixel_mask{nullptr};
        
        // hit map
        MvtxHitMap * m_hit_map{nullptr};

        MvtxRawEvtHeader * m_mvtx_raw_event_header{nullptr};
        MvtxRawHitContainer * m_mvtx_raw_hit_container{nullptr};

        TTree * m_tree{nullptr};
        int m_num_strobes{0};
        int m_num_masked_pixels{0};
        double m_noise_threshold{0.0};
        std::vector<uint64_t> m_masked_pixels{};

        TTree * m_current_mask{nullptr};
        int m_current_nmasked{0};
        double m_current_threshold{0.0};
        std::vector<uint64_t> m_current_masked_pixels{};

        TH1D * m_threshold_vs_nmasked{nullptr};

        int get_nodes(PHCompositeNode* topNode);
        int FillCurrentMaskTree();
        int CalcFHR();

        double calc_threshold(unsigned int nhits);
        
        
};

#endif
