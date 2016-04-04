// Copyright 2015-2016: A Tad Fuzzy

#include <signal.h>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <vedis.h>

#include <iostream>
#include <vector>
#include <cassert>
#include <string>


#include "./server.h"
#include "./singletons.h"

#include "./user/UserFileOperations.h"

#include "JobQueue.h"

#include "model/EntityManager.h"
#include "model/Triple.h"

#include "./platform.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// windows specific to handle ctrl-c call
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
	unsigned int port;
	int loggingLevel;
	std::string dataFilePath;
	std::string configFilePath;

	/*
	*	GET COMMANDLINE ARGUMENTS / READ CONFIG FILE
	*/
	try {

		boost::program_options::options_description genericOptions("generic");
		genericOptions.add_options()			
			("port", boost::program_options::value<unsigned int>(&port)->default_value(1407), "set database port")
			("log", boost::program_options::value<int>(&loggingLevel)->default_value(0), "set logging level")
			("file", boost::program_options::value<std::string>(&dataFilePath), "set data file path");

		boost::program_options::options_description cmdLineOptions("cmdLineOnly");
		cmdLineOptions.add_options()
			("help", "produce help message")
			("config", boost::program_options::value<std::string>(&configFilePath)->default_value("fzdb.cfg"), "set config file");
		cmdLineOptions.add(genericOptions);

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdLineOptions), vm);
		boost::program_options::notify(vm);

		if (vm.count("help")) {
			std::cout << cmdLineOptions << std::endl;
			return 1;
		}

		if (vm.count("config")) {
			configFilePath = vm.at("config").as<std::string>();
		}

		if (!boost::filesystem::exists(configFilePath))
		{
			std::cout << "No config file found" << std::endl;
		}
		else {
			std::ifstream configFile(configFilePath);
			boost::program_options::store(boost::program_options::parse_config_file(configFile, genericOptions), vm);
			boost::program_options::notify(vm);
		}

		if (vm.count("file")) {
			std::cout << "Loading from datafile " << dataFilePath << std::endl;
			Singletons::setDataFilePath(dataFilePath);
		}
	}
	catch (boost::program_options::error& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

  Singletons::initialise();

  /*
  *   INITIALISE LOGGING
  */
  
  try
  {
	  size_t q_size = 1048576; //queue size must be power of 2
	  spdlog::set_async_mode(q_size);

      std::vector<spdlog::sink_ptr> sinks;
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
      if(loggingLevel == 1) sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile", "txt", 23, 59, true));
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
    }
    return TRUE;
}
#endif
