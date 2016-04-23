#include "./session.h"

class CommandInterpreter
{


/**
* @brief Convenience class for interpreting a user's command.
*/
public:

/**
  * @brief Establishes which jobs need doing and then adds jobs to the jobs queue
  *
  * @param session Session object of the current session.
  * @param command Parsed command returned from FSPARQL parser.
  */
  static void ProcessCommand(std::shared_ptr<ISession> session, std::string command);
};
