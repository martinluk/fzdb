#include "./CommandInterpreter.h"

#include "Job.h"
#include "JobQueue.h"

#include "jobs/Ping.h"
#include "jobs/Echo.h"
#include "jobs/Unknown.h"
#include "jobs/Insert.h"
#include "jobs/BGP.h"
#include "jobs/DebugJob.h"
#include "jobs/LoadFileJob.h"
#include "jobs/SaveFileJob.h"
#include "jobs/Link.h"
#include "jobs/Flush.h"

#include "Parser.h"

void CommandInterpreter::ProcessCommand(std::shared_ptr<ISession> session, std::string command) {

    command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
    command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());

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
        case QueryType::DEBUGOTHER:
            JobQueue::AddJob(new DebugJob(session, query.data0));
            break;
        case QueryType::LOAD:
            JobQueue::AddJob(new LoadFileJob(session, query.data0));
            break;
        case QueryType::SAVE:
            JobQueue::AddJob(new SaveFileJob(session, query.data0));
            break;
        case QueryType::LINK:
            JobQueue::AddJob(new jobs::Link(session, query.entities[0], query.entities[1]));
            break;
        case QueryType::UNLINK:
            JobQueue::AddJob(new jobs::Unlink(session, query.entities[0], query.entities[1]));
            break;
        case QueryType::MERGE:
            JobQueue::AddJob(new jobs::Merge(session, query.entities[0], query.entities[1]));
            break;
        case QueryType::FLUSH:
            JobQueue::AddJob(new Flush(session));
            break;
        default:
            JobQueue::AddJob(new UnknownJob(session, command));
        }
    }
    catch (ParseException ex) {
        session->respond(QueryResult::generateError(QueryResult::ErrorCode::ParseError, std::string("Parse error: ") +  ex.what()).toJson());
    }
    catch (std::exception& ex) {
        session->respond(QueryResult::generateError(QueryResult::ErrorCode::GenericError, std::string("Unexpected error: ") + ex.what()).toJson());
    }
}

