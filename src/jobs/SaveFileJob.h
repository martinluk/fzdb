#ifndef JOBS_SAVEFILEJOB_H
#define JOBS_SAVEFILEJOB_H

#include "../Job.h"
#include "QueryResult.h"

class SaveFileJob : public Job
{
public:
    SaveFileJob(std::shared_ptr<ISession> session, const std::string &message);

    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;
    
private:
    mutable std::string    _message;
};

#endif    // JOBS_SAVEFILEJOB_H
