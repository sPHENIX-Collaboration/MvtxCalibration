#include "RawDataManager.h"
#include "RawDataDefs.h"
#include "RawFileFinder.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept> // For exceptions
#include <iomanip>
#include <cstdio> // For popen
#include <regex> // For regex matching
#include <map>
#include <set>

RawDataManager::~RawDataManager()
{
    m_selected_subsystems.clear();
    m_files.clear();
}

void RawDataManager::SelectSubsystems(const std::set<std::string> &subsystems)
{
    m_selected_subsystems.clear();
    std::vector<std::string> available_subsystems = GetAvailableSubsystems();
    for (const auto& subsys : subsystems)
    {
        if(std::find(available_subsystems.begin(), available_subsystems.end(), subsys) == available_subsystems.end())
        {
            std::cerr << "ERROR::RawDataManager::SelectSubsystems - Invalid subsystem: " << subsys << std::endl;
            throw std::runtime_error("ERROR::RawDataManager::SelectSubsystems - Invalid subsystem: " + subsys);
            exit(1);
        }
        m_selected_subsystems.push_back(subsys);
    }

    if(m_selected_subsystems.empty())
    {
        std::cerr << "ERROR::RawDataManager::SelectSubsystems - No valid subsystems selected" << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::SelectSubsystems - No valid subsystems selected");
        exit(1);
    }

    if (Verbosity() > 0)
    {
        std::cout << "INFO::RawDataManager::SelectSubsystems - Selected subsystems: ";
        for (const auto& subsys : m_selected_subsystems)
        {
            std::cout << subsys << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::string> RawDataManager::GetSelectedSubsystems()
{
    // see if subsystems have been selected
    if(m_selected_subsystems.empty())
    {
       m_selected_subsystems = GetAvailableSubsystems();
    }

    return m_selected_subsystems;
}

std::vector<std::string> RawDataManager::GetActiveChannels(const std::string &subsys)
{
    if(m_selected_subsystems.empty())
    {
        std::cout << "INFO::RawDataManager::GetActiveChannels - No subsystems selected, selecting all available subsystems" << std::endl;
        m_selected_subsystems = GetAvailableSubsystems();
    }

    std::vector<std::string> active_channels{};
    // make sure subsys is valid
    if(RawDataDefs::SUBSYS_CHANNEL_MAP.find(subsys) == RawDataDefs::SUBSYS_CHANNEL_MAP.end() && subsys != RawDataDefs::ALL)
    {
        std::cerr << "ERROR::RawDataManager::ActiveChannels - Invalid subsystem: " << subsys << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::ActiveChannels - Invalid subsystem: " + subsys);
        exit(1);
    }
    std::vector<std::string> channels{};

    if(m_files.empty())
    {
        std::cout << "INFO::RawDataManager::GetActiveChannels - No files registered, registering files" << std::endl;
        RegisterFiles();
    }
    if(subsys == RawDataDefs::ALL)
    {
       for (const auto& it : RawDataDefs::ALL_CHANNELS_VEC)
       {
           channels.push_back(it);
       }
    }
    else
    {
        for (const auto &it : RawDataDefs::SUBSYS_CHANNEL_MAP)
        {
            if(it.first == subsys)
            {
                channels = it.second;
                break;
            }
        }
    }
    for (const auto& channel : channels)
    {
        if(m_files.find(channel) != m_files.end())
        {
            active_channels.push_back(channel);
        }
    }

    return active_channels;

}

std::vector<std::string> RawDataManager::GetFiles(const std::string &subsys, const std::string &server_id)
{
    if(m_selected_subsystems.empty())
    {
        std::cout << "INFO::RawDataManager::GetFiles - No subsystems selected, selecting all available subsystems" << std::endl;
        m_selected_subsystems = GetAvailableSubsystems();
    }
    if(m_files.empty())
    {
        std::cout << "INFO::RawDataManager::GetFiles - No files registered, registering files" << std::endl;
        RegisterFiles();
    }



    std::vector<std::string> files{};
    // make sure subsys is valid
    if(RawDataDefs::SUBSYS_CHANNEL_MAP.find(subsys) == RawDataDefs::SUBSYS_CHANNEL_MAP.end())
    {
        std::cerr << "ERROR::RawDataManager::GetFiles - Invalid subsystem: " << subsys << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::GetFiles - Invalid subsystem: " + subsys);
        exit(1);
    }
    std::vector<std::string> channels{};
    
    for (const auto &it : RawDataDefs::SUBSYS_CHANNEL_MAP)
    {
        if(it.first == subsys)
        {
            channels = it.second;
            break;
        }
    }
   
   // make sure server_id is valid
    if(server_id != RawDataDefs::ALL)
    {
        if(std::find(channels.begin(), channels.end(), server_id) == channels.end())
        {
            std::cerr << "ERROR::RawDataManager::GetFiles - Invalid server_id: " << server_id << std::endl;
            throw std::runtime_error("ERROR::RawDataManager::GetFiles - Invalid server_id: " + server_id);
            exit(1);
        }
        channels.clear();
        channels.push_back(server_id);
    }

    for (const auto& channel : channels)
    {
        if(m_files.find(channel) != m_files.end())
        {
            for (const auto& file : m_files[channel])
            {
                files.push_back(file);
            }
        }
    }

    if(files.empty())
    {
        std::cerr << "ERROR::RawDataManager::GetFiles - No files found for subsystem " << subsys << " and server_id " << server_id << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::GetFiles - No files found for subsystem " + subsys + " and server_id " + server_id);
        exit(1);
    }

    return files;
}

std::vector<std::string> RawDataManager::RegisterActiveChannels()
{
    std::vector<std::string> active_channels{};
    std::stringstream ss;
    ss << std::setw(8) << std::setfill('0') << m_run_number;
    std::string run_number_str = ss.str();
    // iterate through RawDataDefs::SUBSYS_CHANNEL_MAP
    for(auto it = RawDataDefs::SUBSYS_CHANNEL_MAP.begin(); it != RawDataDefs::SUBSYS_CHANNEL_MAP.end(); ++it)
    {
        std::vector<std::string> channels = it->second;
        for (const auto& channel : channels)
        {
            bool active_channel = false;
            std::string cmd = "ls " + m_data_path + "/" + it->first + "/" + m_run_type + "/*" + channel + "*-" + run_number_str + "-0000*";
            // make the command have zero stderr output
            cmd += " 2>/dev/null";
            FILE *fp = popen(cmd.c_str(), "r");
            // see if file exists
            if (!fp)
            {
                std::cerr << "ERROR::RawDataManager::RegisterSubsystems - Failed to execute command: " << cmd << std::endl;
                throw std::runtime_error("ERROR::RawDataManager::RegisterSubsystems - Failed to execute command: " + cmd);
                exit(1);
            }
            char path[1024];
            while (fgets(path, sizeof(path), fp) != NULL)
            {
                active_channel = true;
                break;
            }
            pclose(fp);
            if(active_channel)
            {
                active_channels.push_back(channel);
            }
        } 
    }

    if(active_channels.empty())
    {
        std::cerr << "ERROR::RawDataManager::RegisterSubsystems - No active channels found for run " << run_number_str << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::RegisterSubsystems - No active channels found for run " + run_number_str);
        exit(1);
    }

    if (Verbosity() > 0)
    {
        std::vector<std::string> all_subsys{};
        for (const auto& channel : active_channels)
        {
            std::string subsys;
            for (const auto& it : RawDataDefs::SUBSYS_CHANNEL_MAP)
            {
                if(std::find(it.second.begin(), it.second.end(), channel) != it.second.end())
                {
                    subsys = it.first;
                    break;
                }
            }
            if(std::find(all_subsys.begin(), all_subsys.end(), subsys) == all_subsys.end())
            {
                all_subsys.push_back(subsys);
            }
        }
        std::cout << "INFO::RawDataManager::RegisterSubsystems - Active subsystems: ";
        if(Verbosity() > 1)
        {
            std::cout << std::endl;
        }
        
        for (const auto& subsys : all_subsys)
        {
            if(Verbosity() > 1)
            {
                std::cout << "\t";
            }
            std::cout << subsys << " ";
            if (Verbosity() > 1)
            {
                std::cout << " : [";
                std::vector<std::string> channels{};
                for (const auto& it : RawDataDefs::SUBSYS_CHANNEL_MAP)
                {
                    if(it.first == subsys)
                    {
                        channels = it.second;
                        break;
                    }
                }
                for (const auto& channel : channels)
                {
                    if(std::find(active_channels.begin(), active_channels.end(), channel) != active_channels.end())
                    {
                        std::cout << channel << " ";
                    }
                }
                std::cout << "]";
            }
            std::cout << std::endl;
        }
    
    }

    return active_channels;
}

std::vector<std::string> RawDataManager::GetAvailableSubsystems()
{
    std::vector<std::string> active_subsystems{};
    std::stringstream ss;
    ss << std::setw(8) << std::setfill('0') << m_run_number;
    std::string run_number_str = ss.str();
    // iterate through RawDataDefs::SUBSYS_CHANNEL_MAP
    for(auto it = RawDataDefs::SUBSYS_CHANNEL_MAP.begin(); it != RawDataDefs::SUBSYS_CHANNEL_MAP.end(); ++it)
    {
        bool active_subsystem = false;
        std::string cmd = "ls " + m_data_path + "/" + it->first + "/" + m_run_type + "/*-" + run_number_str + "-0000*";
        cmd += " 2>/dev/null";
        FILE *fp = popen(cmd.c_str(), "r");
        // see if file exists
        if (!fp)
        {
            std::cerr << "ERROR::RawDataManager::GetAvailableSubsystems - Failed to execute command: " << cmd << std::endl;
            throw std::runtime_error("ERROR::RawDataManager::GetAvailableSubsystems - Failed to execute command: " + cmd);
            exit(1);
        }
        char path[1024];
        while (fgets(path, sizeof(path), fp) != NULL)
        {
            active_subsystem = true;
            break;
        }
        pclose(fp);

        if(active_subsystem)
        {
            active_subsystems.push_back(it->first);
        }
    }


    if(active_subsystems.empty())
    {
        std::cerr << "ERROR::RawDataManager::GetAvailableSubsystems - No active subsystems found for run " << run_number_str << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::GetAvailableSubsystems - No active subsystems found for run " + run_number_str);
        exit(1);
    }

    return active_subsystems;
}

void RawDataManager::RegisterFiles()
{
    // see if subsystems have been selected
    if(m_selected_subsystems.empty())
    {
        std::cout << "INFO::RawDataManager::RegisterFiles - No subsystems selected, selecting all available subsystems" << std::endl;
        m_selected_subsystems = GetAvailableSubsystems();
    }

    if(m_data_path==""||m_run_type==""||m_run_number==0)
    {
        std::cerr << "ERROR::RawDataManager::RegisterFiles - Data path, run type, or run number not set" << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::RegisterFiles - Data path, run type, or run number not set");
        exit(1);
    }
    
    m_files.clear();

    std::vector<std::string> active_channels = RegisterActiveChannels();

    RawFileFinder * m_finder = new RawFileFinder(m_data_path);
    m_finder->SetRunNumber(m_run_number);
    for(auto it = RawDataDefs::SUBSYS_CHANNEL_MAP.begin(); it != RawDataDefs::SUBSYS_CHANNEL_MAP.end(); ++it)
    {
        std::string subsys = it->first;
        for (const auto& ch : it->second)
        {
            if(std::find(active_channels.begin(), active_channels.end(), ch) == active_channels.end())
            {
                continue;
            }
            std::vector<std::string> files = m_finder->GetFiles(subsys, m_run_type, ch);
            
            // make sure channel is not already in m_files
            if(m_files.find(ch) != m_files.end())
            {
                std::cerr << "ERROR::RawDataManager::RawDataManager - Duplicate channel " << ch << " found for run " << m_run_number << std::endl;
                throw std::runtime_error("ERROR::RawDataManager::RawDataManager - Duplicate channel " + ch + " found for run " + std::to_string(m_run_number));
                exit(1);
            }

            // if no files found channel is not active
            if(files.empty())
            {
                continue;
            }

            m_files[ch] = files;

        }
        
    }

    if (m_files.empty())
    {
        std::cerr << "ERROR::RawDataManager::RawDataManager - No files found for run " << m_run_number << std::endl;
        throw std::runtime_error("ERROR::RawDataManager::RawDataManager - No files found for run " + std::to_string(m_run_number));
        exit(1);
    }

    delete m_finder;

    return;
}