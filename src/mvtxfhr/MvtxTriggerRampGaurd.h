#ifndef MVTXFHR_MVTXTRIGGERRAMPGAURD_H
#define MVTXFHR_MVTXTRIGGERRAMPGAURD_H

#include <fun4all/SubsysReco.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <climits>
#include <memory>

#include <TH1.h>


class PHCompositeNode;

class MvtxTriggerRampGaurd : public SubsysReco
{
    public:

        MvtxTriggerRampGaurd(const std::string& name = "MvtxTriggerRampGaurd") : SubsysReco(name) {}
        ~MvtxTriggerRampGaurd() override;

        // standard Fun4All functions
        int InitRun(PHCompositeNode*) override;
        int process_event(PHCompositeNode*) override;
        int End(PHCompositeNode*) override;

        void SaveOutput(const std::string & filename) { m_output_filename = filename; }

        void SetTriggerRate(double rate) { m_trigger_rate = rate; }
       
    private:

        std::string m_output_filename{""};

        uint64_t m_last_strobe_bco{0};
        uint64_t m_current_strobe_bco{0};
        uint64_t m_delta_strobe_bco{0};

        double m_strobe_length{0.0};
        double m_trigger_rate{101000.0}; // 101 kHz
        unsigned int m_delta_bco_target{0};


        int m_strobe_delta_bco_target{0};
        int m_concurrent_strobe_count{0};
        int m_concurrent_strobe_target{1000};
        int m_current_event{0};

        bool m_is_past_trigger_ramp{false};

        TH1 * m_h_strobe_delta_bco{nullptr};
};

#endif
