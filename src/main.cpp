#include <boost/asio.hpp>

#include <iostream>
#include "./server.h"

#include "./Logger.h"
#include "./JobQueue.h"
#include "./singletons.h"

/**
 * @brief Entry point for the application
 * @details [long description]
 * 
 * @param argc Number of command line arguments passed to the application.
 * @param argv Array of argument strings.
 * 
 * @return Error code
 */
int main(int argc, char* argv[]) {

  Logger::Init();
  //Logger::Log(Logger::Level::Info, "Hello World");

  Singletons::initialise();

  try {
    std::cout << "Fuzzy Database v0.1" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    // Create the IO service.
    // This is essentially a link to the OS' IO system.
    boost::asio::io_service io_service;

    // We use a work object to keep the service busy and prevent it from returning.
    boost::asio::io_service::work work(io_service);
    
    // We then initialise the job queue which will handle the jobs
    // we need to do for the connected client.
    // This creates the desired number of threads that will handle the jobs.
    JobQueue::Init(&io_service);

    // Next we create a TCP server. The server listens for information on
    // the specified port and creates sessions when data is received.
    TCPServer s(io_service, 1407);

    std::cout << "Listening on port 1407..." << std::endl << std::endl;
    std::cout << "CTRL-C to stop" << std::endl;

    // Start the IO service running.
    io_service.run(); 

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  JobQueue::Shutdown();
  Singletons::shutdown();
  return 0;
}
