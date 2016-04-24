#ifndef JOBS_UNKNOWNJOB_H
#define JOBS_UNKNOWNJOB_H

#include "../session.h"

#include "../job.h"

#include "../query_result.h"

namespace jobs {
/**
 * @brief Generic "Unknown" job for when an unknown command is invoked.
 */
class Unknown : public Job {
 private:
  std::string _name;

 public:
  Unknown(std::shared_ptr<ISession> session, std::string name);

  virtual bool constOperation() const override {
    return true;
  }
  virtual QueryResult executeConst() const;
};
}
#endif    // JOBS_UNKNOWNJOB_H
