#ifndef JOBS_LINK_H
#define JOBS_LINK_H
#include "../session.h"

#include "../Job.h"
#include "QueryResult.h"
#include "../model/Entity.h"
#include <string>

namespace jobs {

	class Link : public Job
	{
	public:

		Link(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);


		// Inherited via Job
		virtual QueryResult execute() override;

	protected:
		Entity::EHandle_t _entity1;
		Entity::EHandle_t _entity2;
	};

	class Unlink : public Link
	{
	public:
		Unlink(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
		virtual QueryResult execute() override;
	};

	class Merge : public Link
	{
	public:
		Merge(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2);
		virtual QueryResult execute() override;
	};
}

#endif	// JOBS_ECHOJOB_H
