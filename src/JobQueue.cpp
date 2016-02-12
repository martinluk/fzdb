#include "JobQueue.h"

#include "QueryResult.h"

boost::asio::io_service* JobQueue::_service;
boost::thread_group JobQueue::_threads;


void JobQueue::ExecuteJob(Job* job) {
	QueryResult result = job->execute();
  job->Session()->respond(result.toJSON());
	delete job;
}

void JobQueue::Init(boost::asio::io_service* service) {
  _service = service;
  for (std::size_t i = 0; i < THREAD_COUNT; ++i)
    _threads.create_thread(boost::bind(&boost::asio::io_service::run, service));
}

void JobQueue::AddJob(Job* job) {
  _service->post(boost::bind(JobQueue::ExecuteJob, job));
}

void JobQueue::Shutdown() {
  _threads.join_all();
}
