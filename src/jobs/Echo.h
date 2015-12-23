
#include "../session.h"

#include "../Job.h"
#include "QueryResult.h"
#include <string>

class EchoJob : public Job
{
public:

	EchoJob(ISession* session, std::string message);


	// Inherited via Job
	virtual QueryResult execute() override;

private:
	std::string _message;

};
