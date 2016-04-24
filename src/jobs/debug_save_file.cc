#include "./debug_save_file.h"
#include "../model/entity_manager.h"
#include "../types/string.h"
#include "../model/entity_property.h"
#include "../model/graph_serialiser.h"
#include "../file_system.h"

#include <sstream>

using namespace jobs;

QueryResult DebugSaveFile::execute() {
  using namespace model::types;
  std::stringstream log;

  // Create an entity manager.
  EntityManager manager(NULL);

  // Create some entities.
  std::shared_ptr<Entity> e1 = manager.createEntity(std::string());

  std::vector<std::shared_ptr<String>> values;
  values.push_back(std::make_shared<String>("Arnold", 75));
  values.push_back(std::make_shared<String>("Barry", 43));
  values.push_back(std::make_shared<String>("Clarence", 11));
  // e1->insertProperty<String>(new EntityProperty<String>(1, values));

  std::shared_ptr<Entity> e2 = manager.createEntity(std::string());

  values.clear();
  values.push_back(std::make_shared<String>("Alice", 98));
  values.push_back(std::make_shared<String>("Britney", 61));
  values.push_back(std::make_shared<String>("Catherine", 5));
  //  e2->insertProperty<String>(new EntityProperty<String>(2, values));

  log << "Data to be serialised:\n\n" << manager.dumpContents() << "\n";

  // Serialise the graph.
  Serialiser serialiser;
  GraphSerialiser gSerialiser(&manager);
  std::size_t bytes = gSerialiser.serialise(serialiser);
  log << "Graph serialised to " << bytes << " bytes.\n";

  // Write to a file.
  log << "Working directory: " << FileSystem::workingDirectory() << "\n";

  bool success = true;
  try {
    FileSystem::writeFile(FileSystem::workingDirectory() + std::string("/DebugSaveFile.bin"), serialiser);
  } catch (const std::exception &) {
    success = false;
  }

  log << "File write success: " << success << "\n";

  QueryResult result;
  result.setResultDataText(log.str());
  return result;
}
