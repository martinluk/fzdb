#include "AddUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include "../user/UserExceptions.h"

AddUserJob::AddUserJob(std::shared_ptr<ISession> session, const std::string &username, const std::string &password):
    Job(session, PermType::UserOp) {
    _username = username;
    _password = password;
}

QueryResult AddUserJob::executeNonConst()
{
    try {
        //TODO Need to verify username and password are not empty.
        _database->users().addUser(_username, _password, Permission::UserGroup::EDITOR);
    } catch (const std::exception &ex) {
        return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, ex.what());
    }
    
    QueryResult result;
    result.setResultDataText(std::string("User ") + _username + std::string(" successfulyl added."));
    return result;
}
