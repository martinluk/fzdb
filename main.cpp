#include <iostream>
#include <boost/asio.hpp>

#include "server.h"

int main(int argc, char* argv[]) {
  try {
    boost::asio::io_service io_service;
    TCPServer s(io_service, 1407);
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}