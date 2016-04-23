#ifndef JOBS_USER_LOGOUT
#define JOBS_USER_LOGOUT

#include "../isession.h"
#include "../session.h"
#include "../job.h"

#include "../query_result.h"


/**
 * @brief Logs a currently logged-in user out of the database.
 */
class UserLogoutJob : public Job {
public:

    /**
     * @brief Constructor of user log out job.
     *
     * @param session Session object of the current session.
     */
    UserLogoutJob(std::shared_ptr<ISession> session);
    
    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;

private:
    std::shared_ptr<ISession> _session;
};

#endif
