#ifndef FUZZY_JOBS_DELETE 
#define FUZZY_JOBS_DELETE

#include "../job.h"
#include "../model/triple.h"
#include "../parser.h"
#include "../query_result.h"

#include <string>



/**
* @brief A delete query job.
*/
class Delete : public Job {
public:

  Delete(std::shared_ptr<ISession> session, Query query);

  virtual bool constOperation() const override { return false; }
  virtual QueryResult executeNonConst() override;

private:
  Query _query;

};


#endif
