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

        variables.trimEmptyRows();

		

      //run filters against query
      for(auto filter : _query.whereClause.filters) {
		  for (auto iter = variables.begin(); iter != variables.end();) {
			  if (!filter->Test(std::move(variables), std::move(*iter))) {
				  iter = variables.erase(iter);
			  }
			  else {
				  iter++;
			  }
		  }
      }

	  variables.sort();

      //encode result as JSON
        rapidjson::Value val;
        val.SetArray();
        for (auto iter = variables.cbegin(); iter != variables.cend(); iter++) {

            rapidjson::Value val2;
            val2.SetObject();
            bool hasValues = false;

            for (auto iter2 = _query.selectLine.cbegin(); iter2 != _query.selectLine.cend(); iter2++) {

				if (!variables.contains(*iter2))continue;

                auto i = variables.indexOf(*iter2);

                std::shared_ptr<model::types::Base> basePtr = (*iter).at(i).dataPointer();
                
                if (!(bool(basePtr))) continue;

                rapidjson::Value val3;
				val3.SetString((*iter).at(i).dataPointer()->toString().c_str(), result.allocator());        

                rapidjson::Value varName;
                varName.SetString((*iter2).c_str(), result.allocator());
                val2.AddMember(varName, val3, result.allocator());
                hasValues = true;
            }
            if(hasValues)val.PushBack(val2, result.allocator());
        }
//        rapidjson::Value varName;
//        varName.SetString("result", result.allocator());
//        result.setValue(std::move(varName), std::move(val));
        result.setResultDataFsparql(val);
    }
    catch (NotImplementedException ex) {
        result = QueryResult::generateError(QueryResult::ErrorCode::NotImplemented, ex.what());
    }
    

    return result;
}
