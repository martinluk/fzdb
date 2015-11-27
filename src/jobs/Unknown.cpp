#include "Unknown.h"

UnknownJob::UnknownJob(ISession* session, std::string name) : Job(session)
{
  _name = name;
}

void UnknownJob::execute()
{
	_session->respond("Unknown command: " + _name);
}
