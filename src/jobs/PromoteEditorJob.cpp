#include "PromoteEditorJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include <stdexcept>

PromoteEditorJob::PromoteEditorJob(std::shared_ptr<ISession> session, std::string username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult PromoteEditorJob::adminJobBody() {
    QueryResult result;
    try {
        UserGroup group = UserOperation::getUserGroup(_username); //Throws user not exist exception
        if (group != UserGroup::EDITOR) {
        	throw std::runtime_error("Error: given user is not an editor, cannot promote to admin");
		}
    } catch (std::exception exception) {
        result.generateError(exception.what());
        return result;
    }
	UserOperation::changeUserGroup(_username, UserGroup::ADMIN);
    result.setValue("status","0");
    return result;
}
