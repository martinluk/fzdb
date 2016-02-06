#include "./IUserAdminJobs.h"

class PromoteEditorJob : public IUserAdminJobs {
	public:
		PromoteEditorJob(ISession* session, std::string username);
		virtual QueryResult execute() override;
	private:
		std::string _username;
};
