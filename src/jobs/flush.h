#ifndef FUZZY_JOBS_FLUSH
#define FUZZY_JOBS_FLUSH

#include "../job.h"
#include "../model/triple.h"
#include "../parser.h"
#include "../query_result.h"

#include <string>

namespace jobs {
  // Deletes all entities within the database.
  class Flush : public Job
  {
  public:

    Flush(std::shared_ptr<ISession> session);

      virtual bool constOperation() const override { return false; }
      virtual QueryResult executeNonConst() override;
  };
}

#endif
