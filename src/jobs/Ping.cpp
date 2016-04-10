#include "Ping.h"

PingJob::PingJob(std::shared_ptr<ISession> session) : Job(session, PermType::ViewDB)
{
}

QueryResult PingJob::executeConst() const
{
  QueryResult result;
  result.setResultDataText("PONG");
  return result;
}
