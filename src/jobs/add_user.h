#ifndef FUZZY_JOBS_ADD_USER
#define FUZZY_JOBS_ADD_USER

#include "../job.h"

namespace jobs {
/**
 * @brief Adds a new user to the database. Requires admin privileges.
 */
class AddUser : public Job {
 public:
  /**
   * @brief Constructor of the add user job.
   *
   * @param session Session object of current session.
   * @param username Username of the user to be added.
   * @param password Password of the user to be added.
   */
  AddUser(std::shared_ptr<ISession> session, const std::string &username, const std::string &password);

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
