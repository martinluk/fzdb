#include "Delete.h"
#include "../singletons.h"
#include "../model/EntityManager.h"
#include "../Exceptions.h"

Delete::Delete(std::shared_ptr<ISession> session, Query query) : Job(session, PermType::ModifyDB), _query(query)
{
}

QueryResult Delete::executeNonConst() {
	QueryResult result;
	try {
		_database->entityManager().Delete(std::move(_query.whereClause),_query.selectLine);
	}
	catch (MismatchedTypeException ex) {
		return QueryResult::generateError(QueryResult::ErrorCode::TypeMismatch, ex.what());
	}
	result.setResultDataText(std::string("Deleted ") + std::to_string(_query.conditions.triples.size()) + std::string(" triples."));
	return result;
}
