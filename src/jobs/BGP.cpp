#include "BGP.h"
#include "../singletons.h"
#include "../model/EntityManager.h"
#include "../VariableSet.h"

BGP::BGP(ISession* session, Query query) : Job(session), _query(query)
{
}

QueryResult BGP::execute()
{
	QueryResult result;
	//result.setValue("type", "string");
	//result.setValue(std::string("response"), _message);
	VariableSet variables = Singletons::entityManager()->BGP(_query.whereClause.triples);

	return result;
}
