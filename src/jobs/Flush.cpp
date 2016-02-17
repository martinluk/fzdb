#include "Flush.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

#include "../Exceptions.h"

Flush::Flush(std::shared_ptr<ISession> session) : Job(session)
{
}

QueryResult Flush::executeNonConst()
{
	_database->entityManager().clearAll();
	
	QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), std::string("Database cleared"));
    return result;
}
