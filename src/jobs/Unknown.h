
#include "../session.h"

#include "../Job.h"

class UnknownJob : public Job
{
private:
  std::string _name;

public:
  
	UnknownJob(ISession* session, std::string name);
	// Inherited via Job
	virtual void execute() override;

};