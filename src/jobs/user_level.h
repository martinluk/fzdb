#ifndef JOBS_USER_LEVEL
#define JOBS_USER_LEVEL

#include "../isession.h"
#include "../session.h"
#include "../job.h"

#include "../query_result.h"

/**
 * @brief Returns the current level of the user.
 */
class UserLevelJob : public Job {

public:

    /**
     * @brief Constructor of the User Level job.
     *
     * @param session Session object of the current session.
     */
    UserLevelJob(std::shared_ptr<ISession> session);
    
    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;

private:
    std::shared_ptr<ISession> _session;
};

#endif
