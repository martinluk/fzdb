#include "./insert.h"
#include "../singletons.h"
#include "../model/entity_manager.h"

#include "../exceptions.h"
#include <rapidjson/document.h>

using namespace jobs;

Insert::Insert(std::shared_ptr<ISession> session, Query query) 
    : Job(session, PermType::ModifyDB), _query(query)
{
}

QueryResult Insert::executeNonConst()
{
	QueryResult result;

	std::map<std::string, Entity::EHandle_t> insertedEntities;
    try {
       insertedEntities = _database->entityManager().Insert(std::move(_query.conditions), std::move(_query.whereClause), std::move(_query.settings));
    }
    catch (MismatchedTypeException ex) {
        return QueryResult::generateError(QueryResult::ErrorCode::TypeMismatch, ex.what());
    }

	rapidjson::Value val;
	val.SetObject();
	for (auto insertedEntity : insertedEntities) {
		rapidjson::Value val3;
		val3.SetString(std::to_string(insertedEntity.second).c_str(), result.allocator());

		rapidjson::Value varName;
		varName.SetString(insertedEntity.first.c_str(), result.allocator());
		val.AddMember(varName, val3, result.allocator());
	}
    
    
	result.setResultDataFsparql(val);
	result.setInfo(std::string("Inserted ") + std::to_string(_query.conditions.triples.size()) + std::string(" triples."));
    return result;
}
