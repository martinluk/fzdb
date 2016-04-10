#ifndef FUZZY_JOBS_BGP
#define FUZZY_JOBS_BGP

#include "../Job.h"
#include "../model/Triple.h"
#include "../Parser.h"
#include "../QueryResult.h"

#include <string>

// Basic Graph Processing job. Traverses the entity graph and returns matches.
class BGP : public Job
{
public:

  BGP(std::shared_ptr<ISession> session, Query message);

  virtual bool constOperation() const override { return true; }
  virtual QueryResult executeConst() const override;

private:
  Query _query;

};


#endif
