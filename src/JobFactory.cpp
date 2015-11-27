#include "JobFactory.h"

#include "session.h"

#include "jobs/Ping.h"
#include "jobs/Unknown.h"
#include "jobs/Echo.h"
#include "jobs/CreateEntityJob.h"

void JobFactory::Init() {
 
}

Job* JobFactory::createJob(TCPSession* session, std::string const& name) {
  if(name == "PING") return new PingJob(session);
	else if (name == "CREATE") return new CreateEntityJob(session);
  return new UnknownJob(session, name);
}

Job* JobFactory::createJob(TCPSession* session, std::string const& name, std::string const& arg1) {
	if (name == "ECHO") return new EchoJob(session, arg1);
	return new UnknownJob(session, name);
}

Job* JobFactory::createUnknownJob(TCPSession* session, std::string const& name) {
	return new UnknownJob(session, name);
}
