#pragma once
#include "../Job.h"
#include "../ISession.h"
#include "../user/Permission.h"

class IAdminJob : public Job {
	public:
		IAdminJob(ISession* session):Job(session) {
			//Asserting user admin permission.
			UserGroup usergroup = session->getCurrentUserUserGroup();
			Permission::assertUserOpPermission(usergroup);
		};

		virtual QueryResult execute() override;
};
