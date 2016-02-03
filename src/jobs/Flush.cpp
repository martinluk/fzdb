#include "Flush.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

#include "../Exceptions.h"

Flush::Flush(std::shared_ptr<ISession> session) : Job(session)
{
}

QueryResult Flush::execute()
{
	Singletons::entityManager()->clearAll();
	return QueryResult();
}
