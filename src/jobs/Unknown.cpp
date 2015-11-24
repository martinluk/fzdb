#include "Unknown.h"

UnknownJob::UnknownJob(TCPSession* session, std::string name) : Job(session)
{
  _name = name;
}

void UnknownJob::execute()
{
	_session->respond("Unknown command: " + _name + "\n");
}
