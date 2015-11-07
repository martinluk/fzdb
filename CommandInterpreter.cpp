#include "./CommandInterpreter.h"

#include "Job.h"
#include "JobFactory.h"
#include "JobQueue.h"

void CommandInterpreter::ProcessCommand(TCPSession* session, std::string command) {

  //simple commands
  Job* job = JobFactory::createJob(session, command);
  JobQueue::AddJob(job);
}