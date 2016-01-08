#ifndef FUZZY_JOBS_INSERT
#define FUZZY_JOBS_INSERT

#include "../Job.h"
#include "../model/Triple.h"
#include "../Parser.h"
#include "../QueryResult.h"

#include <string>

class Insert : public Job
{
public:

  Insert(ISession* session, Query query);


  // Inherited via Job
  virtual QueryResult execute() override;

private:
  Query _query;

};


#endif