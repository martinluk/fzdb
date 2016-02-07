#include "DebugDumpEntities.h"
#include "../singletons.h"
#include"../model/EntityManager.h"

QueryResult DebugDumpEntities::execute()
{
	QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), Singletons::entityManager()->dumpContents());
    return result;
}
