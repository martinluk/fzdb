#ifndef JOBS_CREATEENTITYJOB_H
#define JOBS_CREATEENTITYJOB_H

#include "../job.h"
#include "../query_result.h"
#include <string>

// Creates a new entity with the given type.
class CreateEntityJob : public Job
{
public:
    CreateEntityJob(std::shared_ptr<ISession> session, const std::string &type);

    virtual bool constOperation() const override { return false; }
    virtual QueryResult executeNonConst() override;
    
private:
    std::string _type;
};

#endif    // JOBS_CREATEENTITYJOB_H