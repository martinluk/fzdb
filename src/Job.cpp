#include "Job.h"
#include "singletons.h"

Job::Job(std::shared_ptr<ISession> session)
{
	_session = session;
	_database = Singletons::database();
}
