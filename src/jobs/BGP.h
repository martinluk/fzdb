#ifndef FUZZY_JOBS_BGP
#define FUZZY_JOBS_BGP

#include "../Job.h"
#include "../model/Triple.h"
#include "../Parser.h"
#include "../QueryResult.h"

#include <string>

class BGP : public Job
{
public:

  BGP(ISession* session, Query message);


  // Inherited via Job
  virtual QueryResult execute() override;

private:
  Query _query;

};


#endif