#include "Unknown.h"

UnknownJob::UnknownJob(ISession* session, std::string name) : Job(session)
{
  _name = name;
}

QueryResult UnknownJob::execute()
{
	QueryResult result;
	result.setValue("type", "err");
	result.setValue("response", "Unknown command: " + _name + "\n");
	return result;
}
