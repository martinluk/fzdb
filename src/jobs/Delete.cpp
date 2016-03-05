#include "Delete.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

#include "../Exceptions.h"

Delete::Delete(std::shared_ptr<ISession> session, Query query) : Job(session), _query(query)
{
}

QueryResult Delete::executeNonConst()
{
	QueryResult result;
	try {
		_database->entityManager().Delete(_query.conditions.triples);
		result.setValue("type", "string");
		result.setValue("response", std::string("Deleted ") + std::to_string(_query.conditions.triples.size()) + std::string(" triples."));
	}
	catch (MismatchedTypeException ex) {
		result = QueryResult::generateError(ex.what());
	}
	return result;
}
