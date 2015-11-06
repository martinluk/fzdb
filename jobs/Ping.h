
#include "../session.h"

#include "../Job.h"

class PingJob : public Job
{
public:

	PingJob(TCPSession* session);


	// Inherited via Job
	virtual void execute() override;

};