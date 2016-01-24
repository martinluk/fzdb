#include "FileSystem.h"
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

#define FZDB_VERSION    1

// File header - this should not be returned to anyone calling these functions.
// They just want their own data.
struct FileHeader
{
    char identifier[4];     // Identifier - should be "FZDB".
    unsigned int version;   // File version.
    std::size_t size;       // Size of all serialised data after the file headers.
};

namespace FileSystem
{
    bool writeFile(const std::string &filename, const char *begin, std::size_t size)
    {
        std::fstream file;

        FileHeader header;
        memset(&header, 0, sizeof(FileHeader));

        header.identifier[0] = 'F';
        header.identifier[1] = 'Z';
        header.identifier[2] = 'D';
        header.identifier[3] = 'B';
        header.version = FZDB_VERSION;
        header.size = size;
        
        try
        {
            file.open(filename.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
            file.write(reinterpret_cast<const char*>(&header), sizeof(FileHeader));
            file.write(begin, size);
        }
        catch (const std::exception &)
        {
            file.close();
            return false;
        }
        
        file.close();
        return true;
        
    }
    
    bool writeFile(const std::string &filename, const Serialiser &serialiser)
    {
        return writeFile(filename, serialiser.cbegin(), serialiser.size());
    }
    
    std::string workingDirectory()
    {		
        return boost::filesystem::current_path().generic_string();
    }
    
    bool readFile(const std::string &filename, char *buffer, std::size_t length)
    {
        std::fstream file;
        
        try
        {
            file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
            file.seekg(0, file.end);
            std::size_t flen = file.tellg();
            file.seekg(0, file.beg);

            if ( flen <= sizeof(FileHeader) )
            {
                file.close();
                return false;
            }

            // Hopefully reading automatically advances the read point in the file?
            FileHeader header;
            memset(&header, 0, sizeof(FileHeader));
            file.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));

            if ( header.identifier[0] != 'F' ||     // Check identifier.
                 header.identifier[1] != 'Z' ||
                 header.identifier[2] != 'D' ||
                 header.identifier[3] != 'B' ||
                 header.version != FZDB_VERSION ||  // Check it's the version we're expecting.
                 length < header.size)              // Check there's enough space to read the file.
            {
                file.close();
                return false;
            }
            
            file.read(buffer, header.size);
        }
        catch (const std::exception &)
        {
            file.close();
            return false;
        }
        
        file.close();
        return true;
    }

    bool readFile(const std::string &filename, std::vector<char> &buffer)
    {
        std::size_t length = dataLength(filename);
        if ( length < 1 )
            return false;

        buffer.resize(length);
        return readFile(filename, buffer.data(), buffer.size());
    }
    
    std::size_t fileLength(const std::string &filename)
    {
        std::fstream file;
        std::size_t length = 0;
        
        try
        {
            file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
            file.seekg(0, file.end);
            length = file.tellg();
        }
        catch (const std::exception &)
        {
            
        }
        
        file.close();
        return length;
    }

    std::size_t dataLength(const std::string &filename)
    {
        std::fstream file;
        std::size_t length = 0;

        try
        {
            file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);

            FileHeader header;
            memset(&header, 0, sizeof(FileHeader));
            file.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));
            length = header.size;
        }
        catch (const std::exception &)
        {

        }

        file.close();
        return length;
    }
}
