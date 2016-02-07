#include "Insert.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

#include "../Exceptions.h"

Insert::Insert(std::shared_ptr<ISession> session, Query query) : Job(session), _query(query)
{
}

QueryResult Insert::execute()
{
	QueryResult result;
	try {
		Singletons::entityManager()->Insert(_query.conditions.triples);
		result.setValue("type", "string");
		result.setValue("response", std::string("Inserted ") + std::to_string(_query.conditions.triples.size()) + std::string(" triples."));
	}
	catch (MismatchedTypeException ex) {
		result = QueryResult::generateError(ex.what());
	}
	return result;
}
