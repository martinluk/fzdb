#ifndef JOBS_LOADFILEJOB_H
#define JOBS_LOADFILEJOB_H

#include "../job.h"
#include "../query_result.h"

namespace jobs {
/**
* @brief Loads a specified database file from disk.
*/
class LoadFile : public Job {
 public:
  LoadFile(std::shared_ptr<ISession> session, const std::string &message);

  virtual bool constOperation() const override {
    return false;
  }
  virtual QueryResult executeNonConst() override;

 private:
  std::string    _message;
};
}
#endif    // JOBS_LOADFILEJOB_H
