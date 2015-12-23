#ifndef JOBS_ECHOJOB_H
#define JOBS_ECHOJOB_H
#include "../session.h"

#include "../Job.h"

#include <string>

class EchoJob : public Job
{
public:

	EchoJob(ISession* session, std::string message);


	// Inherited via Job
	virtual void execute() override;

private:
	std::string _message;

};

#endif	// JOBS_ECHOJOB_H
