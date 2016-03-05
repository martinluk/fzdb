#ifndef FUZZY_JOBS_DELETE 
#define FUZZY_JOBS_DELETE

#include "../Job.h"
#include "../model/Triple.h"
#include "../Parser.h"
#include "../QueryResult.h"

#include <string>

class Delete : public Job {
public:

  Delete(std::shared_ptr<ISession> session, Query query);

  virtual bool constOperation() const override { return false; }
  virtual QueryResult executeNonConst() override;

private:
  Query _query;

};


#endif
