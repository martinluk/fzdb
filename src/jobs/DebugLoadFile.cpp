#include "DebugLoadFile.h"
#include "../model/EntityManager.h"
#include "../model/types/String.h"
#include "../model/EntityProperty.h"
#include "../model/GraphSerialiser.h"
#include "../FileSystem.h"

#include <sstream>
#include <vector>

QueryResult DebugLoadFile::execute()
{
    using namespace model::types;
    std::stringstream log;
    
    // Create an entity manager.
    EntityManager manager;
    
    // Read in the debug file.
    std::vector<char> buffer;
    std::string filename = FileSystem::workingDirectory() + std::string("/DebugSaveFile.bin");
	
	bool success = true;
	try
	{
		FileSystem::readFile(filename, buffer);
	}
	catch (const std::exception &)
	{
		success = false;
	}
	
    if ( !success )
    {
        log << "Unable to read file '" << filename << "'. Have you run SAVEFILE previously?\n";
    }
    else
    {
        log << "Number of bytes read: " << buffer.size() << "\n";
        
        GraphSerialiser gSer(&manager);
        gSer.unserialise(buffer.data());
        
        log << "Data dump from EntityManager:\n\n";
        log << manager.dumpContents();
    }
    
    QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), log.str());
    return result;
}
