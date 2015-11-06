#include "JobFactory.h"

#include "session.h"

#include "jobs/Ping.h"
#include "jobs/Unknown.h"

void JobFactory::Init() {
 
}

Job* JobFactory::createJob(TCPSession* session, std::string const& name) {
  if(name == "PING") return new PingJob(session);
  return new UnknownJob(session, name);
}
