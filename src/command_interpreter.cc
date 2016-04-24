#include "./command_interpreter.h"

#include "./job.h"
#include "./job_queue.h"

#include "./jobs/ping.h"
#include "./jobs/echo.h"
#include "./jobs/unknown.h"
#include "./jobs/insert.h"
#include "./jobs/delete.h"
#include "./jobs/bgp.h"
#include "./jobs/debug_job.h"
#include "./jobs/load_file.h"
#include "./jobs/save_file.h"
#include "./jobs/link.h"
#include "./jobs/flush.h"
#include "./jobs/add_user.h"
#include "./jobs/delete_user.h"
#include "./jobs/demote_admin.h"
#include "./jobs/promote_editor.h"
#include "./jobs/user_login.h"
#include "./jobs/user_logout.h"
#include "./jobs/user_level.h"
#include "./jobs/user_password.h"
#include "./jobs/user_reset.h"

#include "./parser.h"

void CommandInterpreter::ProcessCommand(std::shared_ptr<ISession> session, std::string command) {

  command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
  command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());

  auto tokens = FSparqlParser::Tokenize(command);

  try {
    Query query = FSparqlParser::ParseAll(tokens);

    switch (query.type) {
    case QueryType::PING:
      JobQueue::AddJob(new jobs::Ping(session));
      break;
    case QueryType::DEBUGECHO:
      JobQueue::AddJob(new jobs::Echo(session, query.data0));
      break;
    case QueryType::INSERT:
      JobQueue::AddJob(new jobs::Insert(session, query));
      break;
    case QueryType::DELETECMD:
      JobQueue::AddJob(new jobs::Delete(session, query));
      break;
    case QueryType::SELECT:
      JobQueue::AddJob(new jobs::BGP(session, query));
      break;
    case QueryType::DEBUGOTHER:
      JobQueue::AddJob(new jobs::Debug(session, query.data0));
      break;
    case QueryType::LOAD:
      JobQueue::AddJob(new jobs::LoadFile(session, query.data0));
      break;
    case QueryType::SAVE:
      JobQueue::AddJob(new jobs::SaveFile(session, query.data0));
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
      JobQueue::AddJob(new jobs::Flush(session));
      break;
    case QueryType::USER_ADD:
      JobQueue::AddJob(new jobs::AddUser(session, query.data0, query.data1));
      break;
    case QueryType::USER_DELETE:
      JobQueue::AddJob(new jobs::DeleteUser(session, query.data0));
      break;
    case QueryType::USER_PASSWORD:
      JobQueue::AddJob(new jobs::UserPassword(session, query.data0, query.data1));
      break;
    case QueryType::USER_PROMOTE:
      JobQueue::AddJob(new jobs::PromoteEditor(session, query.data0));
      break;
    case QueryType::USER_DEMOTE:
      JobQueue::AddJob(new jobs::DemoteAdmin(session, query.data0));
      break;
    case QueryType::USER_LOGIN:
      JobQueue::AddJob(new jobs::UserLogin(session, query.data0, query.data1));
      break;
    case QueryType::USER_LOGOUT:
      JobQueue::AddJob(new jobs::UserLogout(session));
      break;
    case QueryType::USER_RESET:
      JobQueue::AddJob(new jobs::ResetPassword(session, query.data0, query.data1));
      break;
    case QueryType::USER_LEVEL:
      JobQueue::AddJob(new jobs::UserLevel(session));
      break;
    default:
      JobQueue::AddJob(new jobs::Unknown(session, command));
    }
  } catch (ParseException ex) {
    session->respond(QueryResult::generateError(QueryResult::ErrorCode::ParseError, std::string("Parse error: ") +  ex.what()).toJson());
  } catch (std::exception& ex) {
    session->respond(QueryResult::generateError(QueryResult::ErrorCode::GenericError, std::string("Unexpected error: ") + ex.what()).toJson());
  }
}

