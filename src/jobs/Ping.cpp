#include "Ping.h"

PingJob::PingJob(ISession* session) : Job(session)
{

}

void PingJob::execute()
{
	_session->respond("Pong\n");
}
