#include "server.h"

TCPServer::TCPServer(boost::asio::io_service& io_service, short port)
	: io_service_(io_service),
	acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{
	TCPSession* new_session = new TCPSession(io_service_);
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&TCPServer::handle_accept, this, new_session,
			boost::asio::placeholders::error));
}

void TCPServer::handle_accept(TCPSession* new_session,
	const boost::system::error_code& error)
{
	if (!error)
	{
		new_session->start();
		new_session = new TCPSession(io_service_);
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&TCPServer::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}
	else
	{
		delete new_session;
	}
}