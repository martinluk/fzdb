#include "BGP.h"
#include "../singletons.h"
#include "../model/EntityManager.h"
#include "../VariableSet.h"
#include "../Exceptions.h"

#include <rapidjson/document.h>

BGP::BGP(std::shared_ptr<ISession> session, Query query) : Job(session), _query(query)
{
}

QueryResult BGP::executeConst() const
{
	QueryResult result;
	try {
      //run BGP
		VariableSet variables = _database->entityManager().BGP(_query.whereClause, _query.settings);

      //run filters against query
      for(auto filter : _query.whereClause.filters) {
		  variables.getData()->erase(std::remove_if(variables.getData()->begin(), variables.getData()->end(), 
			  [&, this](std::vector<VariableSetValue> row) {
			  return !filter->Test(std::move(row), variables.getMetaData());
		  }), variables.getData()->end());
      }

      //encode result as JSON
		auto data = variables.getData();

		rapidjson::Value val;
		val.SetArray();
		for (auto iter = data->cbegin(); iter != data->cend(); iter++) {

			rapidjson::Value val2;
			val2.SetObject();

			for (auto iter2 = _query.selectLine.cbegin(); iter2 != _query.selectLine.cend(); iter2++) {
				auto i = variables.indexOf(*iter2);

				std::shared_ptr<model::types::Base> basePtr = (*iter)[i].dataPointer();
				
				if (!(bool(basePtr))) continue;

				rapidjson::Value val3;
				val3.SetString((*iter)[i].dataPointer()->toString().c_str(), result.allocator());

				rapidjson::Value varName;
				varName.SetString((*iter2).c_str(), result.allocator());
				val2.AddMember(varName, val3, result.allocator());
			}

			val.PushBack(val2, result.allocator());
		}
//		rapidjson::Value varName;
//		varName.SetString("result", result.allocator());
//		result.setValue(std::move(varName), std::move(val));
		result.setResultDataFsparql(val);
	}
	catch (NotImplementedException ex) {
		result = QueryResult::generateError(QueryResult::ErrorCode::NotImplemented, ex.what());
	}
	

	return result;
}
