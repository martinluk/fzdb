#include "./user_reset.h"
#include "../user/user_operation.h"
#include <stdexcept>

using namespace jobs;

ResetPassword::ResetPassword(std::shared_ptr<ISession> session,
                             const std::string &username, const std::string &password): Job(session, PermType::UserOp) {
  _username = username;
  _password = password;
}

QueryResult ResetPassword::executeNonConst() {
  _database->users().changeUserPassword(_username,_password);
  QueryResult result;
  result.setResultDataText(std::string("Password for user ") + _username + std::string(" has been reset successfully."));
  return result;
}
