#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

#include <mvtxfhr/MvtxPixelDefs.h>

#include <trackbase/MvtxDefs.h>
#include <trackbase/TrkrDefs.h>

#include <TFile.h>
#include <TTree.h>

R__LOAD_LIBRARY(libMvtxFHR.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)


void MakeJsonHotPixelMap(const std::string & calibration_file="fhr_calib42641_100000.root",
                        const double target_threshold=10e-8)
{

    std::cout << "MakeJsonHotPixelMap - Reading calibration file " << calibration_file << std::endl;
    TFile * f = new TFile(calibration_file.c_str(), "READ");
    TTree * t = dynamic_cast<TTree*>(f->Get("masked_pixels"));
    int num_strobes = 0;
    int num_masked_pixels = 0;
    double noise_threshold = 0.0;
    std::vector<uint64_t> * masked_pixels = 0;
    t->SetBranchAddress("num_strobes", &num_strobes);
    t->SetBranchAddress("num_masked_pixels", &num_masked_pixels);
    t->SetBranchAddress("noise_threshold", &noise_threshold);
    t->SetBranchAddress("masked_pixels", &masked_pixels);

    int nentries = t->GetEntries();
    int selected_entry = -1;
    for (int i = 0; i < nentries; i++)
    {
        t->GetEntry(i);
        if(noise_threshold < target_threshold)
        {
            selected_entry = i;
            break;
        }
    }

    t->GetEntry(selected_entry);

    std::vector<int> layer{};
    std::vector<int> stave{};
    std::vector<int> chip{};
    std::vector<int> row{};
    std::vector<int> col{};

    for (int i = 0; i < num_masked_pixels; i++)
    {
        uint64_t pixel = masked_pixels->at(i);
        uint32_t hitsetkey = MvtxPixelDefs::get_hitsetkey(pixel);
        uint32_t hitkey = MvtxPixelDefs::get_hitkey(pixel);

        int l = TrkrDefs::getLayer(hitsetkey);
        int s = MvtxDefs::getStaveId(hitsetkey);
        int ch = MvtxDefs::getChipId(hitsetkey);
        int r = MvtxDefs::getRow(hitkey);
        int c = MvtxDefs::getCol(hitkey);

        layer.push_back(l);
        stave.push_back(s);
        chip.push_back(ch);
        row.push_back(r);
        col.push_back(c);

    }
 

    std::cout << "Number of masked pixels: " << num_masked_pixels << std::endl;
    std::cout << "Noise threshold: " << noise_threshold << std::endl;

    std::cout << "Writing masked pixels to json file" << std::endl;
    std::ofstream json_file("masked_pixels.json");
    json_file << "{\n";
    json_file << "  \"num_masked_pixels\": " << num_masked_pixels << ",\n";
    json_file << "  \"noise_threshold\": " << noise_threshold << ",\n";
    json_file << "  \"masked_pixels\": [\n";
    for (int i = 0; i < num_masked_pixels; i++)
    {
        json_file << "    {\n";
        json_file << "      \"layer\": " << layer[i] << ",\n";
        json_file << "      \"stave\": " << stave[i] << ",\n";
        json_file << "      \"chip\": " << chip[i] << ",\n";
        json_file << "      \"row\": " << row[i] << ",\n";
        json_file << "      \"col\": " << col[i] << "\n";
        json_file << "    }";
        if (i < num_masked_pixels - 1)
        {
            json_file << ",";
        }
        json_file << "\n";
    }

    json_file << "  ]\n";
    json_file << "}\n";
    json_file.close();



    gSystem->Exit(0);
}