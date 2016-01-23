#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include "model/Serialiser.h"

namespace FileSystem
{
    // Any existing file is overwritten.
    bool writeFile(const std::string &filename, const Serialiser &serialiser);
    bool writeFile(const std::string &filename, const char* begin, std::size_t size);
    
    bool readFile(const std::string &filename, std::vector<char> &buffer);
    bool readFile(const std::string &filename, char* buffer, std::size_t length);
    
    std::size_t fileLength(const std::string &filename);
    
    std::string workingDirectory();
}

#endif  // FILESYSTEM_H
