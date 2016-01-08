#include "Insert.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

Insert::Insert(ISession* session, Query query) : Job(session), _query(query)
{
}

QueryResult Insert::execute()
{
	QueryResult result;
	//result.setValue("type", "string");
	//result.setValue(std::string("response"), _message);
	Singletons::entityManager()->Insert(_query.conditions.triples);
	return result;
}
