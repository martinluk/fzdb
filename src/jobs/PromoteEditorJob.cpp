#include "PromoteEditorJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

PromoteEditorJob::PromoteEditorJob(ISession* session, std::string username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult PromoteEditorJob::adminJobBody() {
    QueryResult result;
    try {
        UserOperation::changeUserGroup(_username, UserGroup::ADMIN);
    } catch (UserNotExistException exception) {
        result.generateError("User does not exist"  );
        return result;
    }
    result.setValue("status","0");
    return result;
}
