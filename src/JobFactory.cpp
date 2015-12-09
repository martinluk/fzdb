#include "JobFactory.h"

#include "session.h"

#include "jobs/Ping.h"
#include "jobs/Unknown.h"
#include "jobs/Echo.h"

void JobFactory::Init() {
 
}

Job* JobFactory::createJob(ISession* session, std::string const& name) {
  if(name == "PING") return new PingJob(session);
  return new UnknownJob(session, name);
}

Job* JobFactory::createJob(ISession* session, std::string const& name, std::string const& arg1) {
	if (name == "ECHO") return new EchoJob(session, arg1);
	return new UnknownJob(session, name);
}

Job* JobFactory::createUnknownJob(ISession* session, std::string const& name) {
	return new UnknownJob(session, name);
}
