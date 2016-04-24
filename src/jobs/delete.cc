#include "./delete.h"
#include "../singletons.h"
#include "../model/entity_manager.h"
#include "../exceptions.h"

using namespace jobs;

Delete::Delete(std::shared_ptr<ISession> session, Query query) : Job(session, PermType::ModifyDB), _query(query) {
}

QueryResult Delete::executeNonConst() {
  QueryResult result;
  try {
    auto r = _database->entityManager().Delete( std::move(_query.whereClause), std::move(_query.settings));
    std::string str = std::string("Deleted ") + std::to_string(std::get<0>(r)) + " entities "+ std::to_string(std::get<1>(r)) + " properties and "+std::to_string(std::get<2>(r)) + " objects ";
    result.setResultDataText(str);
  } catch (MismatchedTypeException ex) {
    return QueryResult::generateError(QueryResult::ErrorCode::TypeMismatch, ex.what());
  }
  return result;
}
