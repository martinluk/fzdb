#include "JobFactory.h"
#include <unordered_map>

#include "session.h"

#include "jobs/Ping.h"
#include "jobs/Unknown.h"
#include "jobs/Echo.h"
#include "jobs/CreateEntityJob.h"

// Generalisation: keep a hash table that maps command names to factory functions.
// When a new job is required, call the function that matches the name string.
typedef Job* (*JobFactoryFunction) (TCPSession*, const std::vector<std::string>&);

Job* factoryPingJob(TCPSession* session, const std::vector<std::string> &args)
{
	return new PingJob(session);
}

Job* factoryEchoJob(TCPSession* session, const std::vector<std::string> &args)
{
	// TODO: We might want to change echo to give it the raw list,
	// rather than concatenating here.
	std::string output;
	for ( int i = 0; i < args.size(); i++ )
	{
		output += args[i];
	}

	return new EchoJob(session, output);
}

Job* factoryCreateEntityJob(TCPSession* session, const std::vector<std::string> &args)
{
	return new CreateEntityJob(session);
}

typedef std::unordered_map<std::string,JobFactoryFunction> FactoryMap;
FactoryMap factoryMap_;

void JobFactory::Init() {
	 // TODO: Make the name attribute a static property on each Job subclass?
	 factoryMap_.insert(std::make_pair<std::string,JobFactoryFunction>(
	 	std::string("PING"), &factoryPingJob));

	 factoryMap_.insert(std::make_pair<std::string,JobFactoryFunction>(
	 	std::string("ECHO"), &factoryEchoJob));
	
	 factoryMap_.insert(std::make_pair<std::string,JobFactoryFunction>(
	 	std::string("CREATE"), &factoryCreateEntityJob));
}

Job* JobFactory::createJob(TCPSession* session, const std::vector<std::string> &args)
{
	// Argument 0 should always be the command name.
	assert(args.size() < 1);
	if ( args.size() < 1 )
	{
		return new UnknownJob(session, std::string("ERROR_UNNAMED_JOB"));
	}

	// Try to get a factory function for the job.
	FactoryMap::iterator it = factoryMap_.find(args[0]);

	// If there wasn't one, return an unknown job.
	if ( it == factoryMap_.end() )
	{
		return new UnknownJob(session, args[0]);
	}

	// Return the result of calling the function.
	assert(it->second);
	return (it->second)(session, args);
}

// TODO: Deprecate these.
Job* JobFactory::createJob(TCPSession* session, std::string const& name) {
  if(name == "PING") return new PingJob(session);
	else if (name == "CREATE") return new CreateEntityJob(session);
  return new UnknownJob(session, name);
}

Job* JobFactory::createJob(TCPSession* session, std::string const& name, std::string const& arg1) {
	if (name == "ECHO") return new EchoJob(session, arg1);
	return new UnknownJob(session, name);
}

Job* JobFactory::createUnknownJob(TCPSession* session, std::string const& name) {
	return new UnknownJob(session, name);
}
