
#include "../session.h"

#include "../Job.h"

class PingJob : public Job
{
public:

	PingJob(ISession* session);


	// Inherited via Job
	virtual void execute() override;

};
