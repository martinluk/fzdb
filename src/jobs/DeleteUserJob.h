#include "./IUserAdminJobs.h"

class DeleteUserJob : public IUserAdminJobs {
	public:
		DeleteUserJob(ISession* session, std::string username);
		virtual QueryResult execute() override;
	private:
		std::string _username;
};
