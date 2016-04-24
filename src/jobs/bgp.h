#ifndef FUZZY_JOBS_BGP
#define FUZZY_JOBS_BGP

#include "../job.h"
#include "../model/triple.h"
#include "../parser.h"
#include "../query_result.h"

#include <string>

namespace jobs {
/**
* @brief Basic Graph Processing job. Traverses the entity graph and returns matches.
*/
class BGP : public Job {
 public:

  /**
   * @brief Cosntructor of the BGP job.
   *
   * @param session Session object of the current session.
   * @param message Query parsed.
   */
  BGP(std::shared_ptr<ISession> session, Query message);
  virtual bool constOperation() const override {
    return true;
  }
  virtual QueryResult executeConst() const override;

 private:
  Query _query;

};
}
#endif
