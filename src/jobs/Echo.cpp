#include "Echo.h"

EchoJob::EchoJob(ISession* session, std::string message) : Job(session)
{
	_message = message;
}

void EchoJob::execute()
{
	_session->respond(_message + std::string("\n"));
}
