#include "Ping.h"

PingJob::PingJob(TCPSession* session) : Job(session)
{

}

void PingJob::execute()
{
	_session->respond("Pong");
}
