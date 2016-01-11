#include "./CommandInterpreter.h"

#include "Job.h"
#include "JobQueue.h"

#include "jobs/Ping.h"
#include "jobs/Echo.h"
#include "jobs/Unknown.h"
#include "jobs/Insert.h"
#include "jobs/BGP.h"

#include "Parser.h"

void CommandInterpreter::ProcessCommand(TCPSession* session, std::string command) {

	auto tokens = FSparqlParser::Tokenize(command);

	try {
		Query query = FSparqlParser::ParseAll(tokens);

		switch (query.type) {
		case QueryType::PING:
			JobQueue::AddJob(new PingJob(session));
			break;
		case QueryType::DEBUGECHO:
			JobQueue::AddJob(new EchoJob(session, query.data0));
			break;
		case QueryType::INSERT:
			JobQueue::AddJob(new Insert(session, query));
			break;
		case QueryType::SELECT:
			JobQueue::AddJob(new BGP(session, query));
			break;
		default:
			JobQueue::AddJob(new UnknownJob(session, command));
		}
	}
	catch (ParseException ex) {
		session->respond(std::string("Parse error: ") +  ex.what());
	}
}

