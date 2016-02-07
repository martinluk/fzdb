#include "./IUserAdminJobs.h"

class DeleteUserJob : public IUserAdminJobs {
	public:
		DeleteUserJob(ISession* session, std::string username);
        QueryResult adminJobBody() override;
	private:
		std::string _username;
};
