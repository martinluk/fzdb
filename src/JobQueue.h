#ifndef FUZZY_JOBQUEUE
#define FUZZY_JOBQUEUE

#define THREAD_COUNT 2

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Job.h"

class JobQueue
{
private:
  static boost::asio::io_service* _service;
  static boost::thread_group _threads;
  static void ExecuteJob(Job* job);
public:
  static void Init(boost::asio::io_service* service);
  static void AddJob(Job* job);
  static void Shutdown();
};

#endif
