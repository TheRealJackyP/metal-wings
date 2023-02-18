#ifndef RD_CPP_TESTSCHEDULER_H
#define RD_CPP_TESTSCHEDULER_H

#include "scheduler/base/IScheduler.h"

#include <rd_framework_export.h>

namespace rd
{
	/**
	 * \brief simple scheduler, which immediately invoke action on queue, and is always active.
	 */
	class RD_FRAMEWORK_API SimpleScheduler : public IScheduler
	{
	public:
		// region ctor/dtor
		SimpleScheduler() = default;

		virtual ~SimpleScheduler() override = default;
		// endregion

		virtual void flush() override;

		virtual void queue(std::function<void()> action) override;

		virtual bool is_active() const override;
	};
} // namespace rd

#endif	  // RD_CPP_TESTSCHEDULER_H
