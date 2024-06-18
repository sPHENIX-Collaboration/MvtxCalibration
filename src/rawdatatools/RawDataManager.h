// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef RAWDATATOOLS_RAWDATAMANAGER_H
#define RAWDATATOOLS_RAWDATAMANAGER_H
/*
class to manage raw files for all subsystems for a given run
*/
#include "RawDataDefs.h"

#include <string>
#include <vector>
#include <map>
#include <set>

class RawDataManager
{
  public:

    RawDataManager(const std::string &name="RawDataManager"): m_name(name) {}
    ~RawDataManager();
    
    void SetDataPath(const std::string& path) { m_data_path = path; }
    std::string GetDataPath() const { return m_data_path; }

    void SetRunNumber(const int run) { m_run_number = run; }
    int GetRunNumber() const { return m_run_number; }

    void SetRunType(const std::string &run_type) { m_run_type = run_type; }
    std::string GetRunType() const { return m_run_type; }

    void Verbosity(const unsigned int verb) { m_verbosity = verb; }
    unsigned int Verbosity() const { return m_verbosity; }

    void SelectSubsystems(const std::set<std::string> &subsystems);
    std::vector<std::string> GetAvailableSubsystems();
   
    std::vector<std::string> GetSelectedSubsystems();
    std::vector<std::string> GetActiveChannels(const std::string &subsys = RawDataDefs::ALL);

    std::vector<std::string> GetFiles(const std::string &subsys, const std::string &server_id = RawDataDefs::ALL);
    
  private:
    
    std::string m_name{"RawDataManager"};
    std::string m_data_path{""};
    std::string m_run_type{""};
    int m_run_number{0};

    unsigned int m_verbosity{0};

    std::vector<std::string> m_selected_subsystems{};
    std::map<std::string, std::vector<std::string>> m_files{};
    std::vector<std::string> RegisterActiveChannels();
    void RegisterFiles();
};

#endif // RAWDATATOOLS_RAWDATAMANAGER_H