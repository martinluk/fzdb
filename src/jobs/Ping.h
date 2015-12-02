#ifndef JOB_PINGJOB_H
#define JOB_PINGJOB_H
#include "../session.h"

#include "../Job.h"

class PingJob : public Job
{
public:

	PingJob(TCPSession* session);


	// Inherited via Job
	virtual void execute() override;

};

#endif	// JOB_PINGJOB_H
