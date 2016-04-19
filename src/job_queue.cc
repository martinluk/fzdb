#include "./job_queue.h"

#include "./query_result.h"

boost::asio::io_service* JobQueue::_service;
boost::thread_group JobQueue::_threads;

thread_local std::weak_ptr<ISession> JobQueue::_currentSession;

void JobQueue::ExecuteJob(Job* job) {
	_currentSession = job->Session();
    QueryResult result = job->execute();
  job->Session()->respond(result.toJson());
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

std::weak_ptr<ISession> JobQueue::CurrentSession()
{
	return _currentSession;
}
