#include "./user_logout.h"

//User folder includes
#include "../user/permission.h"
#include "../user/user_operation.h"

using namespace jobs;

UserLogout::UserLogout(std::shared_ptr<ISession> session)
  : Job(session,PermType::LoggedInUser) {
  _session=session;
}

QueryResult UserLogout::executeConst() const {

  _database->users().logout(std::move(_session));

  QueryResult result;
  result.setResultDataText("Logged out successfully.");
  return result;
}
