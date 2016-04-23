#include "./user_level.h"

#include "../user/permission.h"

using namespace jobs;

UserLevel::UserLevel(std::shared_ptr<ISession> session) : Job(session,PermType::ViewDB) {
    _session=session;
}

QueryResult UserLevel::executeConst() const {
    Permission::UserGroup group = _session->getUserGroup();
    std::string groupName = Permission::userGroupName(group);
    QueryResult result;
    result.setResultDataText(groupName);
    return result;
}
