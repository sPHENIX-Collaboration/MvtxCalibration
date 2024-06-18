
#include "RawFileFinder.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem> // For directory operations
#include <stdexcept> // For exceptions
#include <iomanip>
#include <cstdio> // For popen
#include <regex> // For regex matching

void RawFileFinder::RegisterDataDir(const std::string & path) 
{   
    // set class variables
    m_data_path = path;
    // Clear the existing subsystems and run types
    m_valid_subsystems.clear();

    // remove tailing / if exists
    if (m_data_path.back() == '/')
    {
        m_data_path.pop_back();
    }

    // Check if the data path exists
    if (!std::filesystem::exists(m_data_path)) {
        throw std::runtime_error("ERROR::Data path does not exist: " + m_data_path);
        exit(1);
    }

    // Check if it's a directory
    if (!std::filesystem::is_directory(m_data_path)) {
        throw std::runtime_error("ERROR::Data path is not a directory: " + m_data_path);
        exit(1);
    }

    

    // Iterate over directories in the data path
    for (const auto& entry : std::filesystem::directory_iterator(m_data_path)) {
        // Check if it's a directory
        if (entry.is_directory()) {
            // Get the directory name (run type)
            std::string sub_sys = entry.path().filename().string();
            // Store the run type
            m_valid_subsystems.push_back(sub_sys);
        }
    }
    if(m_valid_subsystems.empty())
    {
        std::cerr << "No subsystems found in data path: " << m_data_path << std::endl;
        throw std::runtime_error("ERROR::No subsystems found in data path: " + m_data_path);
        exit(1);
    }


    return;
}

void RawFileFinder::RegisterSubsys(const std::string & subsys)
{
    // set class variables
    if(subsys == m_sub_sys)
    {
        return;
    }

    m_sub_sys = subsys;
    m_file_list.clear();
    m_valid_runtypes.clear();

    // remove tailing / if exists (shouldn't be there for subsystem but just in case)
    if (m_sub_sys.back() == '/')
    {
        m_sub_sys.pop_back();
    }


    bool isvalid = false;
    // Check if the run type is in the m_valid_runtypes vector
    for (const auto& valid_subsystems : m_valid_subsystems) {
        if (m_sub_sys == valid_subsystems) {
            isvalid = true;
            break;
        }
    }

    if(!isvalid)
    {
      std::cerr << "Invalid subsystem: " << m_sub_sys << std::endl;
      std::cerr << "Valid Subsystems: ";
      for (const auto& valid_subsystems : m_valid_subsystems) 
      {
        std::cerr << valid_subsystems << " ";
      }
      std::cerr << std::endl;
      throw std::runtime_error("ERROR::Invalid subsystem: " + m_sub_sys);
      exit(1);
    }

    

    // Iterate over directories in the data path
    for (const auto& entry : std::filesystem::directory_iterator(m_data_path + "/" + m_sub_sys)) {
        // Check if it's a directory
        if (entry.is_directory()) 
        {
            // Get the directory name (run type)
            std::string run_type = entry.path().filename().string();
            // Store the run type
            m_valid_runtypes.push_back(run_type);
        }
    }
    if(m_valid_runtypes.empty())
    {
        std::cerr << "No run types found in data path: " << m_data_path << "/" << m_sub_sys << std::endl;
        throw std::runtime_error("ERROR::No run types found in data path: " + m_data_path + "/" + m_sub_sys);
        exit(1);
    }

    return ;

}

void RawFileFinder::RegisterRunType(const std::string & runtype)
{
    if (runtype == m_run_type)
    {
        return;
    }
    
    // set class variables
    m_run_type = runtype;
    m_file_list.clear();

    // remove tailing / if exists (shouldn't be there for run type but just in case)
    if (m_run_type.back() == '/')
    {
        m_run_type.pop_back();
    }
 
    bool isvalid = false;
    // Check if the run type is in the m_valid_runtypes vector
    for (const auto& valid_runtype : m_valid_runtypes) {
        if (m_run_type == valid_runtype) {
            isvalid = true;
            break;
        }
    }

    if(!isvalid)
    {
      std::cerr << "Invalid run type: " << m_run_type << std::endl;
      std::cerr << "Valid Run types: ";
      for (const auto& valid_runtype : m_valid_runtypes) {
        std::cerr << valid_runtype << " ";
      }
      std::cerr << std::endl;
      throw std::runtime_error("ERROR::Invalid run type: " + m_run_type);
      exit(1);
    }
    
    return ;
}

void RawFileFinder::FindFiles(const int run) 
{   
    if(m_run_number == run && !m_file_list.empty())
    {
        return;
    }
    
    m_run_number = run;
    m_run_number_str = "0";
    m_file_list.clear();
    
    // Convert the integer run number to a string
    std::stringstream ss;
    ss << std::setw(8) << std::setfill('0') << m_run_number;
    m_run_number_str = ss.str();
    
    // Check if the run number is valid
    if (m_run_number_str == "00000000") {
        throw std::runtime_error("ERROR::Invalid run number: " + std::to_string(m_run_number));
        exit(1);
    }

    std::string cmd = "ls " + m_data_path + "/" + m_sub_sys + "/" + m_run_type + "/*" + m_run_number_str + "*";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error executing command: " << cmd << std::endl;
        throw std::runtime_error("ERROR::Invalid run number: " + std::to_string(m_run_number));
        exit(1);
    }

    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        // Remove newline characters from the buffer
        std::string file_name(buffer);
        file_name.erase(std::remove(file_name.begin(), file_name.end(), '\n'), file_name.end());
        m_file_list.push_back(file_name);
    }

    pclose(pipe);

    if (m_file_list.empty()) {
        std::cerr << "No files found for run number: " << m_run_number_str << std::endl;
        throw std::runtime_error("ERROR::No files found for run number: " + m_run_number_str);
        exit(1);
    }

    return;
}

std::vector<std::string> RawFileFinder::GetFiles(const std::string &subsys, const std::string &runtype, const std::string & server_id) 
{
    // Check if the directory has been checked and files filled
    RegisterSubsys(subsys);
    RegisterRunType(runtype);
    FindFiles(m_run_number);

    if (server_id == "all")
    {
        return m_file_list;
    }

    std::vector<std::string> server_files;
    for (const auto& file : m_file_list) {
        if (file.find(server_id) != std::string::npos)
        {
            server_files.push_back(file);
        }
    }

    const int n_files = server_files.size();
    if (n_files == 0) 
    {
        std::cerr << "No files found for server: " << server_id << std::endl;
        throw std::runtime_error("ERROR::No files found for server: " + server_id);
        exit(1);
    }

    // sort by -000X.evt/prdf
    std::vector<std::string> sorted_files;
    std::string file_base = server_files[0].substr(0, server_files[0].find_last_of("-"));
    std::string file_ext = server_files[0].substr(server_files[0].find_last_of(".") + 1);
    for (int i = 0; i < n_files; i++)
    {
        // make i 4 digits
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << i;
        std::string i_str = ss.str();
        std::string file = file_base + "-" + i_str + "." + file_ext;
        // make sure the file exists
        if (std::find(server_files.begin(), server_files.end(), file) == server_files.end())
        {
            std::cerr << "File not found: " << file << std::endl;
            throw std::runtime_error("ERROR::File not found: " + file);
            exit(1);
        }
        sorted_files.push_back(file);
    }
    return sorted_files;

}


