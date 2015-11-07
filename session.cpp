#include <iostream>
#include <string>

#include "./session.h"
#include "Logger.h"

#include "./CommandInterpreter.h"
#include "./server.h"

using boost::asio::ip::tcp;

// As per http://stackoverflow.com/questions/19887537/how-to-detect-when-a-boost-tcp-socket-disconnects
bool _socketDisconnected(const boost::system::error_code& error)
{
	return error == boost::asio::error::eof || error == boost::asio::error::connection_reset;
}

TCPSession::TCPSession(boost::asio::io_service& io_service, TCPServer* parent)
	: socket_(io_service), parent_(parent)
{
}

tcp::socket& TCPSession::socket() {
  return socket_;
}

/**
 * @brief Starts the TCP session
 * @details Starts the session listening for input from the client
 */
void TCPSession::start() {
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
    boost::bind(&TCPSession::handle_read, this,
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
  if ( _socketDisconnected(error) )
  {
  	parent_->signalSessionTerminated(this);
  	return;
  }
  
  if (!error) {
    std::string _command = std::string(data_).substr(0, bytes_transferred);
	  Logger::Log(Logger::Info, "Recieved command: " + _command);
    CommandInterpreter::ProcessCommand(this, _command);
    
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
      boost::bind(&TCPSession::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  } else {
    // TODO: Handle other errors
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
  } else {
    // TODO: Handle errors
  }
}

/**
 * @brief Send a string to the client for this session
 * @details [long description]
 * 
 * @param response A string to send to the client
 */
void TCPSession::respond(const std::string response) {
  boost::asio::async_write(socket_,
    boost::asio::buffer(response.c_str(), response.length()),
    boost::bind(&TCPSession::handle_write, this,
      boost::asio::placeholders::error));
}
