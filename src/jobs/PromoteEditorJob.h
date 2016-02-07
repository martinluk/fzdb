#include "./IUserAdminJobs.h"

class PromoteEditorJob : public IUserAdminJobs {
	public:
		PromoteEditorJob(ISession* session, std::string username);
        QueryResult adminJobBody() override;
	private:
		std::string _username;
};
