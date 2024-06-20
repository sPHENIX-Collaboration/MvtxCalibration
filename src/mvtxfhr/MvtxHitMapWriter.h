#ifndef MVTXFHR_MVTXHITMAPWRITER_H
#define MVTXFHR_MVTXHITMAPWRITER_H

#include "MvtxPixelDefs.h"
#include "MvtxHitMap.h"

#include <fun4all/SubsysReco.h>

#include <ffarawobjects/MvtxRawEvtHeader.h>
#include <ffarawobjects/MvtxRawHitContainer.h>

#include <TTree.h>

#include <cstdint>
#include <iostream>
#include <string>

class PHCompositeNode;

class MvtxHitMapWriter : public SubsysReco
{
    public:

        MvtxHitMapWriter(const std::string& name = "MvtxHitMapWriter") : SubsysReco(name) {}
        ~MvtxHitMapWriter() override {}

        // standard Fun4All functions
        int InitRun(PHCompositeNode*) override;
        int process_event(PHCompositeNode*) override;
        int End(PHCompositeNode*) override;
        
        void SetOutputfile(const std::string& name) { m_outputfile = name;}
      
    private:

        // optional output
        std::string m_outputfile{"mvtx_hit_map.root"};
       
        uint64_t m_last_strobe{0};
      
        // hit map
        MvtxHitMap * m_hit_map{nullptr};

        MvtxRawEvtHeader * m_mvtx_raw_event_header{nullptr};
        MvtxRawHitContainer * m_mvtx_raw_hit_container{nullptr};

        TTree * m_tree_info{nullptr};
        unsigned int m_num_strobes{0};
        unsigned int m_nhits_total{0};
        unsigned int m_num_fired_pixels{0};

        TTree * m_tree{nullptr};
        std::vector<uint64_t> m_pixels{};
        std::vector<unsigned int> m_nhits{};

        int get_nodes(PHCompositeNode* topNode);
        int FillTree();        
        
};

#endif
