#include "BGP.h"
#include "../singletons.h"
#include "../model/EntityManager.h"
#include "../VariableSet.h"

#include <rapidjson/document.h>

BGP::BGP(ISession* session, Query query) : Job(session), _query(query)
{
}

QueryResult BGP::execute()
{
	QueryResult result;
	//result.setValue("type", "string");
	//result.setValue(std::string("response"), _message);
	VariableSet variables = Singletons::entityManager()->BGP(_query.whereClause.triples);
	auto data = variables.getData();
	
	for (auto iter = data.cbegin(); iter != data.cend(); iter++) {
		rapidjson::Value val;
		val.SetArray();

		for (auto iter2 = iter->second.first.cbegin(); iter2 != iter->second.first.cend(); iter2++) {
			rapidjson::Value val2;
			val2.SetString(iter2->c_str(), result.allocator());
			val.PushBack(val2, result.allocator());
		}

		rapidjson::Value varName;
		varName.SetString(iter->first.c_str(), result.allocator());
		result.setValue(std::move(varName), std::move(val));
	}
	

	return result;
}
