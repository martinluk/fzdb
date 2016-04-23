#ifndef JOBS_USER_PASSWORD
#define JOBS_USER_PASSWORD

#include "../session.h"
#include "../job.h"
#include "../query_result.h"



/**
 * @brief Changes the user password of the current user.
 */
class UserPasswordJob : public Job {
public:

    /**
     * @brief Changes the user password of the current user.
     *
     * @param session Session object of the current session.
     * @param oldpassword Old password of the current user.
     * @param newpassword New password of the current user.
     */
    UserPasswordJob(std::shared_ptr<ISession> session, 
            const std::string &oldpassword,
            const std::string &newpassword);
    virtual bool constOperation() const override { return false; }
    virtual QueryResult executeNonConst() override;

private:
    std::string _oldpassword;
    std::string _newpassword;
    std::shared_ptr<ISession> _session;
};

#endif
