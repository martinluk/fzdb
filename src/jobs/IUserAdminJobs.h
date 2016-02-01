#pragma once
#include "../Job.h"
#include "../ISession.h"
#include "../user/Permission.h"

class IUserAdminJobs : public Job {
	public:
		IUserAdminJobs(ISession* session):Job(session) {
			//Asserting user admin permission.
			UserGroup usergroup = session->getCurrentUserUserGroup();
			Permission::assertUserOpPermission(usergroup);
		};

};
