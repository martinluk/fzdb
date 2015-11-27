#include "./server.h"
#include "./Logger.h"

typedef std::vector<ISession*> SessionVector;

TCPServer::TCPServer(boost::asio::io_service& io_service, unsigned short port)
		: port_(port),
    io_service_(io_service),
		// The acceptor is connected to the IO service here.
		// The acceptor listens on the given port to see whether
		// there are any clients who want to establish a
		// connection.
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port_)) {
	
		// Start listening for the first connection request.
		listenForNewConnection();
}

TCPServer::~TCPServer()
{
	// Delete all live sessions.
	while ( liveSessions_.size() > 0 )
	{
		ISession* s = liveSessions_.back();
		s->terminate();
		delete s;
		liveSessions_.pop_back();
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

  	// TODO: How do we get the client's IP to display here?
  	Logger::Log() << "Session initiated." << std::endl;

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
	// Create a new TCP session.
	// This is kept in the live sessions list.
	ISession* s = new TCPSession(io_service_, this);
	liveSessions_.push_back(s);

	// Set up the server's acceptor to call the function
	// below whenever a client tries to connect.
  acceptor_.async_accept(s->socket(),
    boost::bind(&TCPServer::handle_accept, this, s,
      boost::asio::placeholders::error));
}

void TCPServer::signalSessionTerminated(ISession* session)
{
	// Delete the session and remove it fromt the list.
	SessionVector::iterator it = liveSessions_.end();
	for ( SessionVector::iterator i = liveSessions_.begin(); i != liveSessions_.end(); ++i )
	{
		if ( *i == session )
		{
			it = i;
			break;
		}
	}

	if ( it != liveSessions_.end() )
	{
		Logger::Log() << "Session " << (*it) << " deleted." << std::endl;
		delete *it;
		liveSessions_.erase(it);
	}
}
