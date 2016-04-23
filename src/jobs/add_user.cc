#include "./add_user.h"
#include "../user/user_attributes.h"
#include "../user/hashing.h"
#include "../user/user_operation.h"
#include "../user/user_exceptions.h"

using namespace jobs;

AddUser::AddUser(std::shared_ptr<ISession> session, const std::string &username, const std::string &password):
    Job(session, PermType::UserOp) {
    _username = username;
    _password = password;
}

QueryResult AddUser::executeNonConst()
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
