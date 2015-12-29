#include <boost/asio.hpp>

#include <iostream>
#include "./server.h"

#include "model/EntityManager.h"
#include "./singletons.h"
#include "./JobFactory.h"
#include "JobQueue.h"
#include "spdlog/spdlog.h"
#include <vector>

#include "model/Triple.h"

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

  /*
  *   DEFAULT SETTINGS
  */
  unsigned int port = 1407;
  int loggingLevel = 0;

  Singletons::initialise();

  Singletons::entityManager()->AddProperty("<forename>", 1);
  Singletons::entityManager()->AddProperty("<surname>", 2);

  Entity* e1 = Singletons::entityManager()->createEntity();
  e1->insertProperty(EntityProperty(1, std::vector<PropertyValue>{
	  PropertyValue(Variant("fred"), 80)
  }));
  e1->insertProperty(EntityProperty(2, std::vector<PropertyValue>{
	  PropertyValue(Variant("smith"), 80)
  }));

  Entity* e2 = Singletons::entityManager()->createEntity();
  e2->insertProperty(EntityProperty(1, std::vector<PropertyValue>{
	  PropertyValue(Variant("james"), 80)
  }));

  Entity* e3 = Singletons::entityManager()->createEntity();
  e3->insertProperty(EntityProperty(1, std::vector<PropertyValue>{
	  PropertyValue(Variant("fred"), 80)
  }));
  e3->insertProperty(EntityProperty(2, std::vector<PropertyValue>{
	  PropertyValue(Variant("smoth"), 80),
      PropertyValue(Variant("smith"), 40)
  }));

  std::vector<model::Triple> tripleVector{
	model::Triple("$a", "<forename>", "fred"),
	model::Triple("$a", "<surname>", "smith")
  };
  Singletons::entityManager()->BGP2(tripleVector);

  /*
  *   HANDLE COMMAND LINE ARGUMENTS
  */
  for (int i = 1; i < argc; ++i) {

    if (std::string(argv[i]) == "--log" || std::string(argv[i]) == "-l") {
      if (i + 1 < argc) { // Make sure we aren't at the end of argv!
        auto loggingVar = argv[++i];
        if(std::string(loggingVar) == "file") {
          loggingLevel = 1;
        } else {
          std::cerr << "ERROR: Unknown logging option" << std::endl;
        }          
        continue;
      } else {
        std::cerr << argv[i] <<" option requires one argument." << std::endl;
        return 1;
      }  
    }

    if (std::string(argv[i]) == "--port" || std::string(argv[i]) == "-p") {
      if (i + 1 < argc) { // Make sure we aren't at the end of argv!
        try {
          port = std::stoul(argv[++i]);
        } catch(std::invalid_argument e) {
          std::cerr << "Error: Port must be a valid unsigned integer" << std::endl;
          return 1;
        }
        continue;
      } else { 
        std::cerr << argv[i] <<" option requires one argument." << std::endl;
        return 1;
      }  
    }
  }

  /*
  *   INITIALISE LOGGING
  */

  try
  {
      std::vector<spdlog::sink_ptr> sinks;
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
      if(loggingLevel == 1) sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>("logfile", "txt", 23, 59, true));
      auto combined_logger = std::make_shared<spdlog::logger>("main", begin(sinks), end(sinks));
      spdlog::register_logger(combined_logger);
	  combined_logger.get()->set_level(spdlog::level::trace);
  }
  catch (const spdlog::spdlog_ex& ex)
  {
      std::cout << "Log failed: " << ex.what() << std::endl;
  }

  /*
  *   START THE SERVER
  **/

  try {
    std::cout << "Fuzzy Database v0.1" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
	std::cout << "Listening on port " << port << "..." << std::endl << std::endl;
	std::cout << "CTRL-C to stop" << std::endl;

    // Create the IO service.
    // This is essentially a link to the OS' IO system.
    boost::asio::io_service io_service;

    // We use a work object to keep the service busy and prevent it from returning.
    boost::asio::io_service::work work(io_service);
    
    // We then initialise the job queue which will handle the jobs
    // we need to do for the connected client.
    // This creates the desired number of threads that will handle the jobs.
    JobQueue::Init(&io_service);
	JobFactory::Init();

    // Next we create a TCP server. The server listens for information on
    // the specified port and creates sessions when data is received.
    TCPServer s(io_service, port);

    // Start the IO service running.
    io_service.run(); 

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  JobQueue::Shutdown();
  Singletons::shutdown();
  return 0;
}