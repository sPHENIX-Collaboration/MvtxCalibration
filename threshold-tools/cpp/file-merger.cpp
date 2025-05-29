#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

#define CONCAT_INTERNAL(x, y) x##y
#define CONCAT(x, y) CONCAT_INTERNAL(x, y)

// Macro to concatenate file content
#define CONCATENATE_FILES(output_file, input_files) \
do { \
    std::cout << "Concatenating files to " << output_file << std::endl; \
    std::ofstream output(output_file, std::ios::binary); \
    if (!output.is_open()) { \
        std::cerr << "Error: Unable to open output file " << output_file << std::endl; \
        break; \
    } \
    for (const auto& file : input_files) { \
        std::cout << "Staging file " << file << std::endl; \
        std::ifstream input(file, std::ios::binary); \
        if (!input.is_open()) { \
            std::cerr << "Error: Unable to open input file " << file << std::endl; \
            continue; \
        } \
        output << input.rdbuf(); \
        std::cout << "Staged file " << file << std::endl; \
        input.close(); \
    } \
    std::cout << "Concatenated files to " << output_file << std::endl; \
    output.close(); \
    std::cout << "Closed output file " << output_file << std::endl; \
} while(0)


#define FILE_LIST_TO_VECTOR(file_list, output_vector) \
do { \
    std::cout << "Reading file list: " << file_list << std::endl; \
    std::ifstream file(file_list); \
    if (!file.is_open()) { \
        std::cerr << "Error: Unable to open file list: " << file_list << std::endl; \
        break; \
    } \
    std::string line; \
    while (std::getline(file, line)) { \
        if (!line.empty()) { \
            output_vector.push_back(line); \
        } \
    } \
    file.close(); \
} while(0)

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <filelist> <output>" << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    FILE_LIST_TO_VECTOR(argv[1], files);

    // Print the contents of the vector
    std::cout << "Files in the list:" << std::endl;
    for (const std::string& filename : files)
    {
        std::cout << filename << std::endl;
    }

    CONCATENATE_FILES(argv[2], files);

    std::cout << "Done" << std::endl;
    
    return 0;
}

