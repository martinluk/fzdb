
#include "../session.h"

#include "../Job.h"

#include <string>

class EchoJob : public Job
{
public:

	EchoJob(TCPSession* session, std::string message);


	// Inherited via Job
	virtual void execute() override;

private:
	std::string _message;

};