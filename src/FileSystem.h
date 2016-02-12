#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <exception>
#include "model/Serialiser.h"

namespace FileSystem
{
	class FileFormatError : public std::exception
	{
		virtual const char* what() const throw()
		{
			return "File format was not valid";
		}
	};

    // Any existing file is overwritten.
    void writeFile(const std::string &filename, const Serialiser &serialiser);
    void writeFile(const std::string &filename, const char* begin, std::size_t size);
    
    void readFile(const std::string &filename, char* buffer, std::size_t length);
    void readFile(const std::string &filename, std::vector<char> &buffer);
    
    std::size_t fileLength(const std::string &filename);    // Size of file, including extra headers.
    std::size_t dataLength(const std::string &filename);    // Size of data serialised within file.
    
    std::string workingDirectory();
}

#endif  // FILESYSTEM_H
