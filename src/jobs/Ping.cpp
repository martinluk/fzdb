#include "Ping.h"

PingJob::PingJob(ISession* session) : Job(session)
{

}

QueryResult PingJob::execute()
{
  QueryResult result;
  result.setValue("type", "string");
  result.setValue("response", std::string("PONG"));
  return result;
}
