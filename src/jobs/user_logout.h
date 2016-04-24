#ifndef JOBS_USER_LOGOUT
#define JOBS_USER_LOGOUT

#include "../isession.h"
#include "../session.h"
#include "../job.h"

#include "../query_result.h"

namespace jobs {
/**
 * @brief Logs a currently logged-in user out of the database.
 */
class UserLogout : public Job {
 public:
  UserLogout(std::shared_ptr<ISession> session);

  virtual bool constOperation() const override {
    return true;
  }
  virtual QueryResult executeConst() const override;

 private:
  std::shared_ptr<ISession> _session;
};
}
#endif
