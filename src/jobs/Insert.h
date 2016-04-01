#ifndef FUZZY_JOBS_INSERT
#define FUZZY_JOBS_INSERT

#include "../Job.h"
#include "../model/Triple.h"
#include "../Parser.h"
#include "../QueryResult.h"

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
