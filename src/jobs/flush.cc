#include "./flush.h"
#include "../singletons.h"
#include "../model/entity_manager.h"

#include "../exceptions.h"

using namespace jobs;

Flush::Flush(std::shared_ptr<ISession> session) : Job(session, PermType::UserOp)
{
}

QueryResult Flush::executeNonConst()
{
    _database->entityManager().clearAll();
    
    QueryResult result;
    result.setResultDataText("Database cleared.");
    return result;
}
