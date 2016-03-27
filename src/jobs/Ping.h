#ifndef JOB_PINGJOB_H
#define JOB_PINGJOB_H
#include "../session.h"

#include "../Job.h"

#include "QueryResult.h"

// Debugging command. Responds with "PONG".
class PingJob : public Job
{
public:

    PingJob(std::shared_ptr<ISession> session);

    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;

};

#endif    // JOB_PINGJOB_H
