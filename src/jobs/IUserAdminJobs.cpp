#include "IUserAdminJobs.h"

IUserAdminJobs::IUserAdminJobs(std::shared_ptr<ISession> session):Job(session) {
}

bool IUserAdminJobs::hasAdminPermissions() const
{
	return _database->users().getUserGroup(_session->username()) == Permission::UserGroup::ADMIN;
}

QueryResult IUserAdminJobs::errorNoAdminPermissions()
{
	return QueryResult::generateError("Running an admin job without admin permission.");
}
