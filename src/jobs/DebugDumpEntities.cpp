#include "DebugDumpEntities.h"
#include "../singletons.h"
#include"../model/EntityManager.h"
#include "../model/Database.h"

QueryResult DebugDumpEntities::execute(const DebugJob &j)
{
	QueryResult result;
    result.setResultDataText(j._database->entityManager().dumpContents());
    return result;
}
