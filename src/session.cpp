#include <iostream>
#include <string>

#include <boost/uuid/uuid.hpp>

#include "./session.h"
#include "spdlog/spdlog.h"

#include "./CommandInterpreter.h"
#include "./server.h"

using boost::asio::ip::tcp;

// As per http://stackoverflow.com/questions/19887537/how-to-detect-when-a-boost-tcp-socket-disconnects
bool _socketDisconnected(const boost::system::error_code& error)
{
	return error == boost::asio::error::eof || error == boost::asio::error::connection_reset;
}

TCPSession::TCPSession(boost::asio::io_service& io_service, TCPServer* parent, boost::uuids::uuid identifier)
	: _socket(io_service), _parent(parent)
{
	_uuid = identifier;
}

TCPSession::~TCPSession() {
	try {
		_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		_socket.close();
		spdlog::get("main")->info("[{:<}] {:<30}", _uuid, "Session deleted.");
	}
	catch (std::exception ex) {
		std::cout << ex.what() << std::endl;
	}
}

tcp::socket& TCPSession::socket() {
	return _socket;
}

/**
* @brief Starts the TCP session
* @details Starts the session listening for input from the client
*/
void TCPSession::start() {
	_socket.async_read_some(boost::asio::buffer(_data, max_length),
		boost::bind(&TCPSession::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

/**
* @brief Handles the event where the client has sent data to the server
* @details [long description]
*
* @param error Contains an error code if there was an error listening to the client
* @param bytes_transferred The size of the data that has been put into the '_data' buffer
*/
void TCPSession::handle_read(const boost::system::error_code& error,
	size_t bytes_transferred) {

	// Check whether the socket disconnected.
	if (_socketDisconnected(error))
	{
		// No need to call terminate() since we're already disconnected.
		//_parent->signalSessionTerminated(shared_from_this());
		return;
	}

	if (!error) {
		std::string _command = std::string(_data).substr(0, bytes_transferred);
		//Logger::Instance()->Log() << std::setw(37) << _uuid << "Recieved command: " << _command << std::endl;
		spdlog::get("main")->info("[{:<}] {} {}", _uuid, "Recieved command:", _command);

		CommandInterpreter::ProcessCommand(shared_from_this(), _command);

		_socket.async_read_some(boost::asio::buffer(_data, max_length),
			boost::bind(&TCPSession::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		// TODO: Handle other errors.
		// For now, just quit the session.
		//terminate();
		//_parent->signalSessionTerminated(shared_from_this());
	}
}

/**
* @brief Is called whenever a write operation completes
* @details [long description]
*
* @param error If an error occured in the write operation it is here
*/
void TCPSession::handle_write(const boost::system::error_code& error) {
	if (!error) {
	}
	else {
		// TODO: Handle errors.
		// For now, just terminate.
		// _parent->signalSessionTerminated(shared_from_this());
	}
}

/**
* @brief Send a string to the client for this session
* @details [long description]
*
* @param response A string to send to the client
*/
void TCPSession::respond(const std::string response) {
	boost::asio::async_write(_socket,
		boost::asio::buffer(response.c_str(), response.length()),
		boost::bind(&TCPSession::handle_write, this,
			boost::asio::placeholders::error));
}

//void TCPSession::terminate()
//{
//	try {
//		_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
//		_socket.close();
//	}
//	catch (std::exception ex) {
//		std::cout << ex.what() << std::endl;
//	}
//}

boost::uuids::uuid TCPSession::uuid()
{
	return _uuid;
}