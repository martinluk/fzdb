#include "DemoteAdminJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

DemoteAdminJob::DemoteAdminJob(ISession* session, std::string username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult DemoteAdminJob::adminJobBody() {
    QueryResult result;
    try {
        UserOperation::changeUserGroup(_username, UserGroup::EDITOR);
    } catch (UserNotExistException exception) {
        result.generateError("User does not exist");
        return result;
    }
    result.setValue("status","0");
    return result;
}
