#include "Echo.h"

EchoJob::EchoJob(ISession* session, std::string message) : Job(session)
{
	_message = message;
}

QueryResult EchoJob::execute()
{
	QueryResult result;
	result.setValue("type", "string");
	result.setValue(std::string("response"), _message);
	return result;
}
