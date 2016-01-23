#include "FileSystem.h"
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

namespace FileSystem
{
    bool writeFile(const std::string &filename, const char *begin, std::size_t size)
    {
        std::fstream file;
        
        try
        {
            file.open(filename.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
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
    
    bool readFile(const std::string &filename, std::vector<char> &buffer)
    {
        std::fstream file;
        
        try
        {
            file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
            file.seekg(0, file.end);
            int length = file.tellg();
            file.seekg(0, file.beg);
            
            buffer.resize(length);
            file.read(buffer.data(), length);
        }
        catch (const std::exception &)
        {
            file.close();
            return false;
        }
        
        file.close();
        return true;
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
            
            if ( length < flen )
                flen = length;
            
            file.read(buffer, flen);
        }
        catch (const std::exception &)
        {
            file.close();
            return false;
        }
        
        file.close();
        return true;
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
}
