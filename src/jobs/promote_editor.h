#ifndef FUZZY_JOBS_PROMOTE_EDITOR
#define FUZZY_JOBS_PROMOTE_EDITOR

#include "../job.h"

namespace jobs {
/**
 * @brief Promotes an editor to an admin. Requires admin privileges.
 */
class PromoteEditor : public Job {
 public:
  PromoteEditor(std::shared_ptr<ISession> session, const std::string &username);

  virtual bool constOperation() const override {
    return false;
  }
  virtual QueryResult executeNonConst() override;

 private:
  std::string _username;
};
}

#endif
