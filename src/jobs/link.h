#ifndef JOBS_LINK_H
#define JOBS_LINK_H
#include "../session.h"

#include "../job.h"
#include "../query_result.h"
#include "../model/entity.h"
#include <string>

namespace jobs {
    
    class Link : public Job
    {

    /**
    * @brief Links two entities together. 
    */

    public:

        Link(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
        
        virtual bool constOperation() const override { return false; }
        virtual QueryResult executeNonConst() override;

    protected:
        Entity::EHandle_t _entity1;
        Entity::EHandle_t _entity2;
    };

    class Unlink : public Link
    {

    /**
    * @brief Unlinks two previously linked entities. 
    */
    public:
        Unlink(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
        virtual QueryResult executeNonConst() override;
    };

    ///
    /// Merges two entities. This is like linking except it cannot be undone.
    ///
    class Merge : public Link
    {
    public:
        Merge(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
        virtual QueryResult executeNonConst() override;
    };
}

#endif    // JOBS_ECHOJOB_H
