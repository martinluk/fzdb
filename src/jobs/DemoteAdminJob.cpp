#include "DemoteAdminJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

DemoteAdminJob::DemoteAdminJob(std::shared_ptr<ISession> session, const std::string &username)
    :Job(session,PermType::UserOp) {
    _username = username;
}

QueryResult DemoteAdminJob::executeNonConst()
{
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
