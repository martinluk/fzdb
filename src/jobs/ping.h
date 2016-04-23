#ifndef JOB_PINGJOB_H
#define JOB_PINGJOB_H
#include "../session.h"

#include "../job.h"

#include "../query_result.h"

class PingJob : public Job

/**
* @brief Debugging command. Responds with "PONG". 
**/
{
public:

    PingJob(std::shared_ptr<ISession> session);

    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;

};

#endif    // JOB_PINGJOB_H
