#ifndef JOBS_LINK_H
#define JOBS_LINK_H
#include "../session.h"

#include "../Job.h"
#include "QueryResult.h"
#include "../model/Entity.h"
#include <string>

namespace jobs {
    
    // Links two entities together.
    class Link : public Job
    {
    public:

        Link(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
        
        virtual bool constOperation() const override { return false; }
        virtual QueryResult executeNonConst() override;

    protected:
        Entity::EHandle_t _entity1;
        Entity::EHandle_t _entity2;
    };

    // Unlinks two previously linked entities.
    class Unlink : public Link
    {
    public:
        Unlink(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
        virtual QueryResult executeNonConst() override;
    };

    // Merges two entities. This is like linking except it cannot be undone.
    class Merge : public Link
    {
    public:
        Merge(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
        virtual QueryResult executeNonConst() override;
    };
}

#endif    // JOBS_ECHOJOB_H
