#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include "model/Serialiser.h"

namespace FileSystem
{
    // Any existing file is overwritten.
    bool writeFile(const std::string &filename, const Serialiser &serialiser);
    bool writeFile(const std::string &filename, const char* begin, std::size_t size);
    
    std::string workingDirectory();
}

#endif  // FILESYSTEM_H
