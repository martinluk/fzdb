#include <iostream>

#include "session.h"

using boost::asio::ip::tcp;

tcp::socket& TCPSession::socket()
{
	return socket_;
}

void TCPSession::start()
{
	socket_.async_read_some(boost::asio::buffer(data_, max_length),
		boost::bind(&TCPSession::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void TCPSession::handle_read(const boost::system::error_code& error,
	size_t bytes_transferred)
{
	if (!error)
	{
		_command = std::string(data_).substr(0, bytes_transferred);
		if (_command == "PING") {
			std::cout << "PONG" << std::endl;
			respond("PONG");
		}
		else {
			std::cout << "Unknown command: " << _command << "\n";
			respond("Unknown command: " + _command);
		}				
		_command = "";

		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&TCPSession::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		delete this;
	}
}

void TCPSession::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
	}
	else
	{
		delete this;
	}
}

void TCPSession::respond(const std::string response)
{
	boost::asio::async_write(socket_,
		boost::asio::buffer(response.c_str(), response.length()),
		boost::bind(&TCPSession::handle_write, this,
			boost::asio::placeholders::error));
}
