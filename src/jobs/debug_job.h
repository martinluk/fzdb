#ifndef JOB_DEBUGJOB_H
#define JOB_DEBUGJOB_H
#include "../session.h"

#include "../job.h"

#include "../query_result.h"

namespace jobs {
/**
* @brief Generic debug job.  Depending on the command arguments, we do various debug tasks here.
*/
class Debug : public Job {
  friend class DebugDumpEntities;
 public:
  Debug(std::shared_ptr<ISession> session, const std::string &message);

  virtual bool constOperation() const override {
    return true;
  }
  virtual QueryResult executeConst() const override;

 private:
  std::string _message;
};
}

#endif    // JOB_DEBUGJOB_H
