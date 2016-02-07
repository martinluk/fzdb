#include "./IUserAdminJobs.h"

class DemoteAdminJob : public IUserAdminJobs {
	public:
		DemoteAdminJob(ISession* session, std::string username);
        QueryResult adminJobBody() override;
	private:
		std::string _username;
};
