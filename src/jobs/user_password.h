#ifndef JOBS_USER_PASSWORD
#define JOBS_USER_PASSWORD

#include "../session.h"
#include "../job.h"
#include "../query_result.h"

namespace jobs {
/**
 * @brief Changes the user password of the current user.
 */
class UserPassword : public Job {
 public:
  UserPassword(std::shared_ptr<ISession> session,
               const std::string &oldpassword,
               const std::string &newpassword);
  virtual bool constOperation() const override {
    return false;
  }
  virtual QueryResult executeNonConst() override;
 private:
  std::string _oldpassword;
  std::string _newpassword;
  std::shared_ptr<ISession> _session;
};
}

#endif
