/*
*    Parses commands, establishes which jobs need doing and then adds jobs to the jobs queue
*/
#include "./session.h"

// Convenience class for interpreting a user's command.
// The command is parsed using the FSPARQL parser and the correct job
// is instanciated and executed.
class CommandInterpreter
{
public:
  static void ProcessCommand(std::shared_ptr<ISession> session, std::string command);
};