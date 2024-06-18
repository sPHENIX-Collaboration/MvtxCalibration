#include "RawDataDefs.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

const std::string RawDataDefs::get_channel_name(const std::string &subsys, const unsigned int server_id)
{
    // find subsystem in SUBSYS_CHANNEL_MAP
    auto it = RawDataDefs::SUBSYS_CHANNEL_MAP.find(subsys);
    if(it == RawDataDefs::SUBSYS_CHANNEL_MAP.end())
    {
        std::cerr << "Invalid subsystem: " << subsys << std::endl;
        throw std::runtime_error("ERROR::Invalid subsystem: " + subsys);
        exit(1);
    }
    // get the vector of channels for the subsystem
    std::vector<std::string> channels = it->second;
    if (server_id > channels.size())
    {
        std::cerr << "Invalid server id: " << server_id << std::endl;
        throw std::runtime_error("ERROR::Invalid server id: " + std::to_string(server_id));
        exit(1);
    }

    return channels.at(server_id);

}


