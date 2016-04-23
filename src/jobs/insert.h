#ifndef FUZZY_JOBS_INSERT
#define FUZZY_JOBS_INSERT

#include "../job.h"
#include "../model/triple.h"
#include "../parser.h"
#include "../query_result.h"

#include <string>

class Insert : public Job
{

  /**
  * @brief Inserts a set of triples into the database.
  */
public:

  Insert(std::shared_ptr<ISession> session, Query query);

  virtual bool constOperation() const override { return false; }
  virtual QueryResult executeNonConst() override;

private:
  Query _query;

};


#endif
