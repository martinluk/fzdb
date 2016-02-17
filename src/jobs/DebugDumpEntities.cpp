#include "DebugDumpEntities.h"
#include "../singletons.h"
#include"../model/EntityManager.h"
#include "../model/Database.h"

QueryResult DebugDumpEntities::execute(const DebugJob &j)
{
	QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), j._database->entityManager().dumpContents());
    return result;
}
