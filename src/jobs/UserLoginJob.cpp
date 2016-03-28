#include "UserLoginJob.h"

//User folder includes
#include "../user/Permission.h"
#include "../user/UserOperation.h"

UserLoginJob::UserLoginJob(std::shared_ptr<ISession> session, const std::string &username, const std::string &password) : Job(session) {
    _username=username;
    _password=password;
    _session=session;
};

QueryResult UserLoginJob::executeNonConst()
{
    try {
        //TODO Verify username and password are not empty
        _database->users().login(std::move(_session), _username,_password);
    } catch (const LoginUnsuccessfulException &ex) { 
        return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, ex.what());
    }
    
    QueryResult result;
    result.setResultDataText("Logged in successfully.");
    return result;
}
