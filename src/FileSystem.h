#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include "model/Serialiser.h"

namespace FileSystem
{
    // TODO: Create exceptions to be thrown - returning a bool isn't all that informative.

    // Any existing file is overwritten.
    bool writeFile(const std::string &filename, const Serialiser &serialiser);
    bool writeFile(const std::string &filename, const char* begin, std::size_t size);
    
    bool readFile(const std::string &filename, char* buffer, std::size_t length);
    bool readFile(const std::string &filename, std::vector<char> &buffer);
    
    std::size_t fileLength(const std::string &filename);    // Size of file, including extra headers.
    std::size_t dataLength(const std::string &filename);    // Size of data serialised within file.
    
    std::string workingDirectory();
}

#endif  // FILESYSTEM_H
