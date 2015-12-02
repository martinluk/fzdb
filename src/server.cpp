#include "./server.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "spdlog/spdlog.h"

using SessionVector = std::vector<ISession*>;

TCPServer::TCPServer(boost::asio::io_service& io_service, unsigned short port) :
	_port (port),
    _io_service(io_service),
		// The acceptor is connected to the IO service here.
		// The acceptor listens on the given port to see whether
		// there are any clients who want to establish a
		// connection.
    _acceptor(io_service, tcp::endpoint(tcp::v4(), _port)) {
	
		// Start listening for the first connection request.
		listenForNewConnection();

		_uuidGenerator = boost::uuids::random_generator();
}

TCPServer::~TCPServer()
{
	// Delete all live sessions.
	while ( _liveSessions.size() > 0 )
	{
		ISession* s = _liveSessions.back();
		s->terminate();
		delete s;
		_liveSessions.pop_back();
	}
}

// Called when the acceptor receives a connection request.
// No actual data about the request is read in here -
// this function just sets up a TCP session which then
// handles reading and acting upon the actual data from the
// client.
void TCPServer::handle_accept(ISession* session, const boost::system::error_code& error) {
	// If no errors in the setup:
  if (!error) {
	  // Create a new TCP session.
	  // This is kept in the live sessions list.	 

  	// TODO: How do we get the client's IP to display here?
  	spdlog::get("main")->info("[{:<}] {:<30}", session->uuid(), "Session initiated.");// << std::setw(37) << session->uuid() << "Session initiated." << std::endl;
    //spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name) function");
		// Start the session. It will sit in our vector until we decide to remove it.
    session->start();

		// Listen for the next connection attempt.
		listenForNewConnection();
  } else {
		// Nothing to do here now.
  }
}

void TCPServer::listenForNewConnection()
{
	ISession* s = new TCPSession(_io_service, this, _uuidGenerator());
	_liveSessions.push_back(s);

	// Set up the server's acceptor to call the function
	// below whenever a client tries to connect.
  _acceptor.async_accept(s->socket(),
    boost::bind(&TCPServer::handle_accept, this, s,
      boost::asio::placeholders::error));
}

void TCPServer::signalSessionTerminated(ISession* session)
{
	// Delete the session and remove it fromt the list.
	SessionVector::iterator it = _liveSessions.end();
	for ( SessionVector::iterator i = _liveSessions.begin(); i != _liveSessions.end(); ++i )
	{
		if ( *i == session )
		{
			it = i;
			break;
		}
	}

	if ( it != _liveSessions.end() )
	{		
		spdlog::get("main")->info("[{:<}] {:<30}", session->uuid(), "Session deleted.");
		delete *it;
		_liveSessions.erase(it);
	}
}
