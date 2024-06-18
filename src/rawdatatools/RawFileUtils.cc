#include "RawFileUtils.h"

#include <string>
#include <fstream>
#include <iostream>

bool RawFileUtils::isGood(const std::string &infile)
{
    std::ifstream fin;
    fin.open(infile);
    bool good_file = false;
    if (fin.is_open())
    {
        if (fin.peek() != std::ifstream::traits_type::eof()) // is it non zero?
        {
            good_file = true;
        }
        fin.close();
    }
    if(!good_file)
    {
        std::cout << "File " << infile << " is not good" << std::endl;
    }
    
    return good_file;
}
