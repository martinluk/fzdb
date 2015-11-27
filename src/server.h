#ifndef FUZZYDB_SERVER
#define FUZZYDB_SERVER

#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "./ISession.h"
#include "./session.h"

using boost::asio::ip::tcp;

class TCPServer
{
	friend class TCPSession;
public:
	TCPServer(boost::asio::io_service& io_service, unsigned short port);
	~TCPServer();

	void handle_accept(ISession* session, const boost::system::error_code& error);

private:
	void listenForNewConnection();
	void signalSessionTerminated(ISession* session);

	unsigned short port_;
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	std::vector<ISession*> liveSessions_;
};

#endif
