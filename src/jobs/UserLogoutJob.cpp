#include "UserLogoutJob.h"

//User folder includes
#include "../user/Permission.h"
#include "../user/UserOperation.h"

UserLogoutJob::UserLogoutJob(std::shared_ptr<ISession> session) 
    : Job(session,PermType::LoggedInUser) {
    _session=session;
}

QueryResult UserLogoutJob::executeConst() const {
    _session->clearCurrentUserName();
    _session->_userId=0;
    _session->setUserGroup(Permission::UserGroup::GUEST);
    
    QueryResult result;
    result.setResultDataText("Logged out successfully.");
    return result;
}
