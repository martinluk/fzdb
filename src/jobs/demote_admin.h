#ifndef FUZZY_JOBS_DEMOTE_ADMIN
#define FUZZY_JOBS_DEMOTE_ADMIN

#include "../job.h"

namespace jobs {
/**
 * @brief Demotes an admin back to an editor. Requires admin privileges.
 */
class DemoteAdmin : public Job {
 public:
  DemoteAdmin(std::shared_ptr<ISession> session, const std::string &username);

  virtual bool constOperation() const override {
    return false;
  }
  virtual QueryResult executeNonConst() override;

 private:
  std::string _username;
};
}
#endif
