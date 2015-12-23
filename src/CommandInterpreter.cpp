#include "./CommandInterpreter.h"

#include "Job.h"
#include "JobFactory.h"
#include "JobQueue.h"

#include "jobs/Ping.h"
#include "jobs/Echo.h"
#include "jobs/Unknown.h"

#include "Parser.h"

void CommandInterpreter::ProcessCommand(ISession* session, std::string command) {

	auto tokens = Tokenize(command);

	try {
		Query query = ParseAll(tokens);

		switch (query.type) {
		case QueryType::PING:
			JobQueue::AddJob(new PingJob(session));
			break;
		case QueryType::DEBUGECHO:
			JobQueue::AddJob(new EchoJob(session, query.data0));
			break;
		default:
			JobQueue::AddJob(new UnknownJob(session, command));
		}
	}
	catch (ParseException ex) {
		session->respond(std::string("Parse error: ") +  ex.what());
	}
}
