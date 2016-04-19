/****************************** Module Header ******************************\
* Module Name:  SampleService.cpp
* Project:      CppWindowsService
* Copyright (c) Microsoft Corporation.
* 
* Provides a sample service class that derives from the service base class - 
* CServiceBase. The sample service logs the service start and stop 
* information to the Application event log, and shows how to run the main 
* function of the service in a thread pool worker thread.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/en-us/openness/resources/licenses.aspx#MPL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <signal.h>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <vedis.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <string>


#include "./server.h"
#include "./singletons.h"

#include "./user/user_file_operations.h"

#include "./job_queue.h"

#include "./model/entity_manager.h"
#include "./model/triple.h"

#include "./platform.h"

#pragma region Includes
#include "./sample_service.h"
#include "./thread_pool.h"
#include <Wtsapi32.h>
#pragma comment( lib, "Wtsapi32.lib" )
#pragma endregion

boost::asio::io_service* pIOService = NULL;

CSampleService::CSampleService(PWSTR pszServiceName, 
                               BOOL fCanStop, 
                               BOOL fCanShutdown, 
                               BOOL fCanPauseContinue)
: CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
    m_fStopping = FALSE;

    // Create a manual-reset event that is not signaled at first to indicate 
    // the stopped signal of the service.
    m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStoppedEvent == NULL)
    {
        throw GetLastError();
    }
}


CSampleService::~CSampleService(void)
{
    if (m_hStoppedEvent)
    {
        CloseHandle(m_hStoppedEvent);
        m_hStoppedEvent = NULL;
    }
}


//
//   FUNCTION: CSampleService::OnStart(DWORD, LPWSTR *)
//
//   PURPOSE: The function is executed when a Start command is sent to the 
//   service by the SCM or when the operating system starts (for a service 
//   that starts automatically). It specifies actions to take when the 
//   service starts. In this code sample, OnStart logs a service-start 
//   message to the Application log, and queues the main service function for 
//   execution in a thread pool worker thread.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
//   NOTE: A service application is designed to be long running. Therefore, 
//   it usually polls or monitors something in the system. The monitoring is 
//   set up in the OnStart method. However, OnStart does not actually do the 
//   monitoring. The OnStart method must return to the operating system after 
//   the service's operation has begun. It must not loop forever or block. To 
//   set up a simple monitoring mechanism, one general solution is to create 
//   a timer in OnStart. The timer would then raise events in your code 
//   periodically, at which time your service could do its monitoring. The 
//   other solution is to spawn a new thread to perform the main service 
//   functions, which is demonstrated in this code sample.
//
void CSampleService::OnStart(DWORD dwArgc, LPWSTR *lpszArgv)
{
	wchar_t title[] = L"mrnservice in startup - 60 seconds to take action";
	wchar_t message[] = L"To debug, first attach to the process with Visual "
		L"Studio, then click OK. If you don't want to debug, "
		L"just click OK without attaching";
	DWORD consoleSession = ::WTSGetActiveConsoleSessionId();
	DWORD response;
	BOOL ret = ::WTSSendMessage(WTS_CURRENT_SERVER_HANDLE,
		consoleSession,
		title, sizeof(title),
		message, sizeof(message),
		MB_OK,
		60,
		&response,
		TRUE);

    // Log a service start message to the Application log.
    WriteEventLogEntry(L"CppWindowsService in OnStart", 
        EVENTLOG_INFORMATION_TYPE);

    // Queue the main service function for execution in a worker thread.
    CThreadPool::QueueUserWorkItem(&CSampleService::ServiceWorkerThread, this);
	Singletons::setDataFilePath("C:/data.dat");
	Singletons::initialise();

	size_t q_size = 1048576;  // queue size must be power of 2
	spdlog::set_async_mode(q_size);

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
	int loggingLevel = 0;
	if (loggingLevel == 1)
		sinks.push_back(
			std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile",
				"txt", 23, 59, true));

	auto combined_logger = std::make_shared<spdlog::logger>("main",
		begin(sinks), end(sinks));

	spdlog::register_logger(combined_logger);
	combined_logger.get()->set_level(spdlog::level::trace);

	// Create the IO service.
	// This is essentially a link to the OS' IO system.
	boost::asio::io_service io_service;
	pIOService = &io_service;

	// We use a work object to keep the service busy and prevent it
	// from returning.
	boost::asio::io_service::work work(io_service);

	// We then initialise the job queue which will handle the jobs
	// we need to do for the connected client.
	// This creates the desired number of threads that will handle the jobs.
	JobQueue::Init(&io_service);

	// Next we create a TCP server. The server listens for information on
	// the specified port and creates sessions when data is received.
	TCPServer s(io_service, 1407);
}


//
//   FUNCTION: CSampleService::ServiceWorkerThread(void)
//
//   PURPOSE: The method performs the main function of the service. It runs 
//   on a thread pool worker thread.
//
void CSampleService::ServiceWorkerThread(void)
{
    // Periodically check if the service is stopping.
    while (!m_fStopping)
    {
        // Perform main service function here...
		if(pIOService)pIOService->run();
        ::Sleep(2000);  // Simulate some lengthy operations.
    }

    // Signal the stopped event.
    SetEvent(m_hStoppedEvent);
}


//
//   FUNCTION: CSampleService::OnStop(void)
//
//   PURPOSE: The function is executed when a Stop command is sent to the 
//   service by SCM. It specifies actions to take when a service stops 
//   running. In this code sample, OnStop logs a service-stop message to the 
//   Application log, and waits for the finish of the main service function.
//
//   COMMENTS:
//   Be sure to periodically call ReportServiceStatus() with 
//   SERVICE_STOP_PENDING if the procedure is going to take long time. 
//
void CSampleService::OnStop()
{
    // Log a service stop message to the Application log.
    WriteEventLogEntry(L"CppWindowsService in OnStop", 
        EVENTLOG_INFORMATION_TYPE);

    // Indicate that the service is stopping and wait for the finish of the 
    // main service function (ServiceWorkerThread).
    m_fStopping = TRUE;

	JobQueue::Shutdown();
	Singletons::shutdown();
	pIOService = NULL;

    if (WaitForSingleObject(m_hStoppedEvent, INFINITE) != WAIT_OBJECT_0)
    {
        throw GetLastError();
    }


}