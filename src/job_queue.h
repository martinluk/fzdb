#ifndef FUZZY_JOBQUEUE
#define FUZZY_JOBQUEUE

#define THREAD_COUNT 1

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "./job.h"

// Queue of jobs to be executed. Has capability for multithreading.
class JobQueue
{
private:
  static boost::asio::io_service* _service;
  static boost::thread_group _threads;
  static void ExecuteJob(Job* job);
  static thread_local std::weak_ptr<ISession> _currentSession;
public:
  static void Init(boost::asio::io_service* service);
  static void AddJob(Job* job);
  static void Shutdown();
  static std::weak_ptr<ISession> CurrentSession();
};

#endif
