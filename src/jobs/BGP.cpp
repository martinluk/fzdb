#include "BGP.h"
#include "../singletons.h"
#include "../model/EntityManager.h"
#include "../VariableSet.h"
#include "../Exceptions.h"

#include <rapidjson/document.h>

BGP::BGP(ISession* session, Query query) : Job(session), _query(query)
{
}

QueryResult BGP::execute()
{
	QueryResult result;
	try {
      //run BGP
		VariableSet variables = Singletons::entityManager()->BGP(_query.whereClause);

      //run filters against query
      for(auto filter : _query.whereClause.filters) {
               
      }

      //encode result as JSON
		auto data = variables.getData();

		rapidjson::Value val;
		val.SetArray();
		for (auto iter = data->cbegin(); iter != data->cend(); iter++) {

			rapidjson::Value val2;
			val2.SetArray();
			for (auto iter2 = iter->cbegin(); iter2 != iter->cend(); iter2++) {
				rapidjson::Value val3;
				val3.SetString(iter2->c_str(), result.allocator());
				val2.PushBack(val3, result.allocator());
			}

			val.PushBack(val2, result.allocator());
		}
		rapidjson::Value varName;
		varName.SetString("result", result.allocator());
		result.setValue(std::move(varName), std::move(val));
	}
	catch (NotImplementedException ex) {
		result = QueryResult::generateError(ex.what());
	}
	

	return result;
}
