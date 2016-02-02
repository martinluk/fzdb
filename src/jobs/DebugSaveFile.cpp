#include "DebugSaveFile.h"
#include "../model/EntityManager.h"
#include "../model/types/String.h"
#include "../model/EntityProperty.h"
#include "../model/GraphSerialiser.h"
#include "../FileSystem.h"

#include <sstream>

QueryResult DebugSaveFile::execute()
{
    /*using namespace model::types;
    std::stringstream log;
    
    // Create an entity manager.
    EntityManager manager;
    
    // Create some entities.
	std::shared_ptr<Entity> e1 = manager.createEntity();
    
    std::vector<std::shared_ptr<String>> values;
    values.push_back(std::make_shared<String>("Arnold", 75));
    values.push_back(std::make_shared<String>("Barry", 43));
    values.push_back(std::make_shared<String>("Clarence", 11));
    e1->insertProperty<String>(new EntityProperty<String>(1, values));
    
	std::shared_ptr<Entity> e2 = manager.createEntity();
    
    values.clear();
    values.push_back(std::make_shared<String>("Alice", 98));
    values.push_back(std::make_shared<String>("Britney", 61));
    values.push_back(std::make_shared<String>("Catherine", 5));
    e2->insertProperty<String>(new EntityProperty<String>(2, values));
    
    log << "Data to be serialised:\n\n" << manager.dumpContents() << "\n";
    
    // Serialise the graph.
    Serialiser serialiser;
    GraphSerialiser gSerialiser(&manager);
    std::size_t bytes = gSerialiser.serialise(serialiser);
    log << "Graph serialised to " << bytes << " bytes.\n";
    
    // Write to a file.
    log << "Working directory: " << FileSystem::workingDirectory() << "\n";
	
	bool success = true;
	try
	{
		FileSystem::writeFile(FileSystem::workingDirectory() + std::string("/DebugSaveFile.bin"), serialiser);
	}
	catch (const std::exception &)
	{
		success = false;
	}
    
    log << "File write success: " << success << "\n";
    */
    QueryResult result;
    result.setValue("type", "string");
    //result.setValue(std::string("response"), log.str());
    return result;
}
