
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

#include <iostream>
#include <vector>

#include <signal.h>
#include <cassert>

#include "./server.h"
#include "./singletons.h"

#include "./user/UserFileOperations.h"

#include "JobQueue.h"

#include "model/EntityManager.h"
#include "model/Triple.h"

#include "platform.h"

//windows specific to handle ctrl-c call
#if PLATFORM == PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

BOOL WINAPI ConsoleHandler(DWORD);
#endif

boost::asio::io_service* pIOService = NULL;

void shutDownApplication()
{
    JobQueue::Shutdown();
    Singletons::shutdown();
}

void sigHandler(int s)
{
    // Ctrl-C is signal 2. Should this even get called if it's a different signal?
    assert(s == 2);
    
    std::cout << "Caught SIGINT, shutting down." << std::endl;
    pIOService->stop();     // This needs to be here before shutting down, or we don't actually stop.
    shutDownApplication();
    exit(0);
}

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

 /*
  *   HANDLE COMMAND LINE ARGUMENTS
  */
  for (int i = 1; i < argc; ++i)
  {
    std::string arg(argv[i]);
    if (arg == "--log" || arg == "-l")
    {
      // Make sure we aren't at the end of argv!
      if (i + 1 < argc)
      {
        auto loggingVar = argv[++i];
        if(std::string(loggingVar) == "file")
        {
          loggingLevel = 1;
        }
        else
        {
          std::cerr << "ERROR: Unknown logging option" << std::endl;
        }          
        continue;
      }
      else
      {
        std::cerr << argv[i] <<" option requires one argument." << std::endl;
        return 1;
      }  
    }

    if (arg == "--port" || arg == "-p")
    {
      // Make sure we aren't at the end of argv!
      if (i + 1 < argc)
      {
        try
        {
          port = std::stoul(argv[++i]);
        }
        catch(std::invalid_argument e)
        {
          std::cerr << "Error: Port must be a valid unsigned integer" << std::endl;
          return 1;
        }
        continue;
      }
      else
      {
        std::cerr << argv[i] <<" option requires one argument." << std::endl;
        return 1;
      }
    }

    if (arg == "--file" || arg == "-f")
    {
      if ( i+1 < argc )
      {
        Singletons::setDataFilePath(std::string(argv[i+1]));
        continue;
      }
      else
      {
        std::cerr << argv[i] <<" option requires one argument." << std::endl;
        return 1;
      }
    }
  }

  Singletons::initialise();

  //Testing
 /* Singletons::entityManager()->linkEntities(0, 1);
  Singletons::entityManager()->linkEntities(1, 2);
  auto bleh = Singletons::entityManager()->getLinkGraph(0, std::set<Entity::EHandle_t>());*/

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
  
#if PLATFORM != PLATFORM_WINDOWS
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = &sigHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
#else
  if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE)) {
	  fprintf(stderr, "Unable to install handler!\n");
	  return EXIT_FAILURE;
  }
#endif

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
    pIOService = &io_service;

    // We use a work object to keep the service busy and prevent it from returning.
    boost::asio::io_service::work work(io_service);
    
    // We then initialise the job queue which will handle the jobs
    // we need to do for the connected client.
    // This creates the desired number of threads that will handle the jobs.
    JobQueue::Init(&io_service);

    // Next we create a TCP server. The server listens for information on
    // the specified port and creates sessions when data is received.
    TCPServer s(io_service, port);

    // Start the IO service running.
    io_service.run(); 

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  shutDownApplication();
  pIOService = NULL;
  return 0;
}

//windows specific to handle ctrl-c call
#if PLATFORM == PLATFORM_WINDOWS
BOOL WINAPI ConsoleHandler(DWORD dwType)
{
	switch (dwType) {
	case CTRL_C_EVENT:
		sigHandler(2);
		break;
	case CTRL_BREAK_EVENT:
		printf("break\n");
		break;
	default:
		printf("Some other event\n");
	}
	return TRUE;
}
#endif
