#include <boost/asio.hpp>

#include <iostream>
#include "./server.h"

#include "./Logger.h"

/**
 * @brief Entry point for the application
 * @details [long description]
 * 
 * @param argc [description]
 * @param argv [description]
 * 
 * @return error code
 */
int main(int argc, char* argv[]) {

	Logger::Init();
	Logger::Log(Logger::Level::Info, "Hello World");

  try {
    std::cout << "Fuzzy Database v0.1" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    boost::asio::io_service io_service;
    TCPServer s(io_service, 1407);
    std::cout << "Listening on port 1407.." << std::endl << std::endl;
    std::cout << "CTRL-C to stop" << std::endl;
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
