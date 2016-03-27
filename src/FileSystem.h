#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <stdexcept>
#include "model/Serialiser.h"

namespace FileSystem
{
    class FileFormatError : public std::runtime_error
    {
    public:
        FileFormatError() : std::runtime_error("File format was not valid")
        {
        }
    };

    // Writes binary output to a file on disk, including the required headers for
    // us to be able to read it again later. Any existing file is overwritten.
    void writeFile(const std::string &filename, const Serialiser &serialiser);
    void writeFile(const std::string &filename, const char* begin, std::size_t size);
    
    // Reads the specified binary file and outputs it to the specified buffer.
    void readFile(const std::string &filename, char* buffer, std::size_t length);
    void readFile(const std::string &filename, std::vector<char> &buffer);
    
    std::size_t fileLength(const std::string &filename);    // Size of file, including extra headers.
    std::size_t dataLength(const std::string &filename);    // Size of data serialised within file (not including headers).
    
    // Returns the working directory of the application.
    std::string workingDirectory();
}

#endif  // FILESYSTEM_H
