#include "Ping.h"

PingJob::PingJob(std::shared_ptr<ISession> session) : Job(session)
{

}

QueryResult PingJob::executeConst() const
{
  QueryResult result;
  result.setValue("type", "string");
  result.setValue("response", std::string("PONG"));
  return result;
}
