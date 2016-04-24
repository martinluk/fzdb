#ifndef FUZZY_JOBS_RESET_PASSWORD
#define FUZZY_JOBS_RESET_PASSWORD

#include "../job.h"
namespace jobs {

/**
 * @brief Resets password of other users - runnable by admin
 */
class ResetPassword : public Job {
 public:
  ResetPassword(std::shared_ptr<ISession> session,
                const std::string &username,
                const std::string &password);

  virtual bool constOperation() const override {
    return false;
  }
  virtual QueryResult executeNonConst() override;

 private:
  std::string _username;
  std::string _password;
};
}

#endif
