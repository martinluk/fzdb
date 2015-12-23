#ifndef FUZZY_JOBS_BGP
#define FUZZY_JOBS_BGP

#include "../Job.h"
#include "../model/Triple.h"
#include "../QueryResult.h"

#include <string>

class BGP : public Job
{
public:

  BGPJob(ISession* session, std::string message);


  // Inherited via Job
  virtual QueryResult execute() override;

private:
  std::string _message;

};


#endif