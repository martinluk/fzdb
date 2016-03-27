#include "./IUserAdminJobs.h"

// Promotes an editor to an admin. Requires admin privileges.
class PromoteEditorJob : public IUserAdminJobs {
    public:
        PromoteEditorJob(std::shared_ptr<ISession> session, const std::string &username);
        
        virtual bool constOperation() const override { return false; }
        virtual QueryResult executeNonConst() override;
        
    private:
        std::string _username;
};
