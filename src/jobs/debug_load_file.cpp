#include "./debug_load_file.h"
#include "../model/entity_manager.h"
#include "../types/string.h"
#include "../model/entity_property.h"
#include "../model/graph_serialiser.h"
#include "../file_system.h"

#include <sstream>
#include <vector>

QueryResult DebugLoadFile::execute()
{
    using namespace model::types;
    std::stringstream log;
    
    // Create an entity manager.
    EntityManager manager(NULL);
    
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
        gSer.unserialise(buffer.data(), buffer.size());
        
        log << "Data dump from EntityManager:\n\n";
        log << manager.dumpContents();
    }
    
    QueryResult result;
    result.setResultDataText(log.str());
    return result;
}
