#include "Insert.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

#include "../Exceptions.h"

Insert::Insert(std::shared_ptr<ISession> session, Query query) : Job(session), _query(query)
{
}

QueryResult Insert::executeNonConst()
{
	try {
		_database->entityManager().Insert(std::move(_query.conditions));
	}
	catch (MismatchedTypeException ex) {
		return QueryResult::generateError(QueryResult::ErrorCode::TypeMismatch, ex.what());
	}
	
	QueryResult result;
	result.setResultDataText(std::string("Inserted ") + std::to_string(_query.conditions.triples.size()) + std::string(" triples."));
	return result;
}
