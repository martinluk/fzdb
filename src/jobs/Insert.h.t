#ifndef FUZZY_JOBS_INSERT
#define FUZZY_JOBS_INSERT

#include "../job.h"
#include "../model/triple.h"
#include "../parser.h"
#include "../query_result.h"

#include <string>

// Insers a set of triples into the database.
class Insert : public Job
{
public:

  Insert(std::shared_ptr<ISession> session, Query query);

  virtual bool constOperation() const override { return false; }
  virtual QueryResult executeNonConst() override;

private:
  Query _query;

};


#endif
