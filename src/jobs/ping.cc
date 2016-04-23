#include "./ping.h"

using namespace jobs;

Ping::Ping(std::shared_ptr<ISession> session) : Job(session, PermType::ViewDB)
{
}

QueryResult Ping::executeConst() const
{
  QueryResult result;
  result.setResultDataText("PONG");
  return result;
}
