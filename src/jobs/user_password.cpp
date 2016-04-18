#include "./user_password.h"
#include "../user/permission.h"
#include "../user/user_operation.h"

UserPasswordJob::UserPasswordJob(std::shared_ptr<ISession> session,
        const std::string &oldpassword,
        const std::string &newpassword) : Job(session,PermType::LoggedInUser) {
    _oldpassword=oldpassword;
    _newpassword=newpassword;
    _session=session;
};

QueryResult UserPasswordJob::executeNonConst()
{
    try {
        _database->users().changeUserPassword(std::move(_session), _oldpassword, _newpassword);
    } catch (const LoginUnsuccessfulException &ex) { 
        return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, ex.what());
    }
    
    QueryResult result;
    result.setResultDataText("Password changed successfully.");
    return result;
}
