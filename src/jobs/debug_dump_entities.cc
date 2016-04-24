#include "./debug_dump_entities.h"
#include "../singletons.h"
#include"../model/entity_manager.h"
#include "../model/database.h"

using namespace jobs;

QueryResult DebugDumpEntities::execute(const Debug &j) {
  QueryResult result;
  result.setResultDataText(j._database->entityManager().dumpContents());
  return result;
}
