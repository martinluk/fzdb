#include "./demote_admin.h"
#include "../user/user_attributes.h"
#include "../user/hashing.h"
#include "../user/user_operation.h"

using namespace jobs;

DemoteAdmin::DemoteAdmin(std::shared_ptr<ISession> session, const std::string &username)
  :Job(session,PermType::UserOp) {
  _username = username;
}

QueryResult DemoteAdmin::executeNonConst() {
  try {
    Permission::UserGroup group = _database->users().getUserGroup(_username); //Throws user not exist exception

    if (group != Permission::UserGroup::ADMIN) {
      return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, "User to demote is not an admin.");
    }
  } catch (const std::exception &ex) {
    return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, ex.what());
  }

  _database->users().changeUserGroup(_username, Permission::UserGroup::EDITOR);

  QueryResult result;
  result.setResultDataText(std::string("User ") + _username + std::string(" demoted to editor."));
  return result;
}
