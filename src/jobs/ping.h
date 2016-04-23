#ifndef JOB_PINGJOB_H
#define JOB_PINGJOB_H
#include "../session.h"

#include "../job.h"

#include "../query_result.h"

namespace jobs {
	// Debugging command. Responds with "PONG".
	class Ping : public Job
	{
	public:

		Ping(std::shared_ptr<ISession> session);

		virtual bool constOperation() const override { return true; }
		virtual QueryResult executeConst() const override;

	};
}
#endif    // JOB_PINGJOB_H
