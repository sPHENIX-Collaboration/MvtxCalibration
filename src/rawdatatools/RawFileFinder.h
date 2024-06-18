// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef RAWDATATOOLS_RAWFILEFINDER_H
#define RAWDATATOOLS_RAWFILEFINDER_H
/*
class to find all prdf files for each subsystem and run type
*/
#include <string>
#include <vector>

class RawFileFinder
{
  public:

    RawFileFinder(const std::string &data_dir) { RegisterDataDir(data_dir);}
    ~RawFileFinder()
    {
      m_file_list.clear();
      m_valid_runtypes.clear();
      m_valid_subsystems.clear();
    }
    
    void SetDataPath(const std::string& path) { RegisterDataDir(path);}
    std::string GetDataPath() const { return m_data_path; }

    void SetRunNumber(const int run) { m_run_number = run; }
    void SetRunNumber(const std::string &run) { m_run_number = std::stoi(run); }
    int GetRunNumber() const { return m_run_number; }

    std::vector<std::string> GetFiles(const std::string &subsys, const std::string &runtype, const std::string & server_id="all");

    std::string GetRunNumberStr() const { return m_run_number_str; }
    std::string GetSubsys() const { return m_sub_sys; }
    std::string GetRunType() const { return m_run_type; }
    
  private:
  

    // data path
    std::string m_data_path{""};
    void RegisterDataDir(const std::string & path); // method to check if data path is valid

    // subsystem
    std::string m_sub_sys{"NONE"};
    std::vector<std::string> m_valid_subsystems {};
    void RegisterSubsys(const std::string & subsys); // method to check if subsystem is valid

    // run type
    std::string m_run_type{"NONE"};
    std::vector<std::string> m_valid_runtypes {};
    void RegisterRunType(const std::string & runtype); // method to check if run type is valid

    // run number
    int m_run_number{0};
    std::string m_run_number_str{"0"};
    void FindFiles(const int run); // method to check if run number is valid

    std::vector<std::string> m_file_list {};

  
}; // class RAWDATATOOLS_RawFileFinder

#endif
