#ifndef JOBS_USER_LEVEL
#define JOBS_USER_LEVEL

#include "../isession.h"
#include "../session.h"
#include "../job.h"

#include "../query_result.h"

namespace jobs {
    /**
     * @brief Returns the current level of the user.
     */
	class UserLevel : public Job {
	public:
		UserLevel(std::shared_ptr<ISession> session);
		virtual bool constOperation() const override { return true; }
		virtual QueryResult executeConst() const override;

	private:
		std::shared_ptr<ISession> _session;
	};
}

#endif
