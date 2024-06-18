// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef RAWDATATOOLS_RAWDATADEFS_H
#define RAWDATATOOLS_RAWDATADEFS_H

#include <string>
#include <vector>
#include <map>
#include <utility>

namespace RawDataDefs
{

  // path to the directories where the data is stored
  static const std::string SPHNXPRO_COMM = "/sphenix/lustre01/sphnxpro/commissioning";
  static const std::string SPHNXPRO_PHYS = "/sphenix/lustre01/sphnxpro/physics";

  // run types
  static const std::string BEAM = "beam";
  static const std::string CALIB = "calib";
  static const std::string COSMIC = "cosmic";
  static const std::string PHYSICS = "physics";
  static const std::string DATA = "data";
  static const std::string JUNK = "junk";

  // subsystems
  static const std::string GL1 = "GL1";
  static const std::string HCAL = "HCal";
  static const std::string INTT = "INTT";
  static const std::string LL1 = "LL1";
  static const std::string MVTX = "MVTX";
  static const std::string TPOT = "TPOT";
  static const std::string ZDC = "ZDC";
  static const std::string EMCAL = "emcal";
  static const std::string MBD = "mbd";
  static const std::string TPC = "tpc";

  static const std::string ALL = "all";


  static const std::map<std::string, std::vector<std::string>> SUBSYS_CHANNEL_MAP =
  {
    {GL1, {"gl1daq"}},
    {MVTX, {"mvtx0", "mvtx1", "mvtx2", "mvtx3", "mvtx4", "mvtx5"}},
    {INTT, {"intt0", "intt1", "intt2", "intt3", "intt4", "intt5", "intt6", "intt7"}},
    {TPC, {"ebdc00", "ebdc01", "ebdc02", "ebdc03", "ebdc04", "ebdc05",
          "ebdc06", "ebdc07", "ebdc08", "ebdc09", "ebdc10", "ebdc11",
          "ebdc12", "ebdc13", "ebdc14", "ebdc15", "ebdc16", "ebdc17", 
          "ebdc18", "ebdc19", "ebdc20", "ebdc21", "ebdc22", "ebdc23"}},
    {TPOT, {"ebdc39"}},
    {EMCAL, {"seb00", "seb01", "seb02", "seb03", "seb04", "seb05", "seb06", "seb07", 
             "seb08", "seb09", "seb10", "seb11", "seb12", "seb13", "seb14", "seb15"}},
    {HCAL, {"seb16", "seb17"}},
    {MBD, {"seb18"}},
    {LL1, {"seb19"}},
    {ZDC, {"seb20"}}
  };

  static const std::vector<std::string> ALL_CHANNELS_VEC = {
    "gl1daq",
    "mvtx0", "mvtx1", "mvtx2", "mvtx3", "mvtx4", "mvtx5",
    "intt0", "intt1", "intt2", "intt3", "intt4", "intt5", "intt6", "intt7",
    "ebdc00", "ebdc01", "ebdc02", "ebdc03", "ebdc04", "ebdc05", "ebdc06", "ebdc07", "ebdc08",
    "ebdc09", "ebdc10", "ebdc11", "ebdc12", "ebdc13", "ebdc14", "ebdc15", "ebdc16", 
    "ebdc17", "ebdc18", "ebdc19", "ebdc20", "ebdc21", "ebdc22", "ebdc23",
    "ebdc39",
    "seb00", "seb01", "seb02", "seb03", "seb04", "seb05", "seb06", "seb07", 
    "seb08", "seb09", "seb10", "seb11", "seb12", "seb13", "seb14", "seb15",
    "seb16", "seb17",
    "seb18",
    "seb19",
    "seb20"
  };
  
  const std::string get_channel_name(const std::string &subsys, const unsigned int server_id = 0);
    
} // namespace RawDataDefs


#endif // RAWDATATOOLS_RAWDATADEFS_H