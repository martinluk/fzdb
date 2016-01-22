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
}
