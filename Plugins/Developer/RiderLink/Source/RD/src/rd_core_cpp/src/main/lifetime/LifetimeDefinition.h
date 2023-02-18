#ifndef RD_CPP_CORE_LIFETIME_DEFINITION_H
#define RD_CPP_CORE_LIFETIME_DEFINITION_H

#include "util/core_traits.h"

#include "LifetimeImpl.h"
#include "Lifetime.h"

#include <functional>
#include <type_traits>

#include <rd_core_export.h>

namespace rd
{
	class RD_CORE_API LifetimeDefinition
	{
	private:
		friend class SequentialLifetimes;

		bool eternaled = false;

	public:
		Lifetime lifetime;

		explicit LifetimeDefinition(bool is_eternal = false);

		explicit LifetimeDefinition(const Lifetime& parent);

		LifetimeDefinition(const LifetimeDefinition& other) = delete;

		LifetimeDefinition& operator=(const LifetimeDefinition& other) = delete;

		LifetimeDefinition(LifetimeDefinition&& other) = default;

		LifetimeDefinition& operator=(LifetimeDefinition&& other) = default;

		virtual ~LifetimeDefinition();

		//    static std::shared_ptr<LifetimeDefinition> eternal;
		static std::shared_ptr<LifetimeDefinition> get_shared_eternal();

		bool is_terminated() const;

		bool is_eternal() const;

		void terminate();

		template <typename F>
		static util::result_of_t<F(Lifetime)> use(F&& block)
		{
			LifetimeDefinition definition(false);
			Lifetime lw = definition.lifetime.create_nested();
			return block(lw);
		}
	};
} // namespace rd

#endif	  // RD_CPP_CORE_LIFETIME_DEFINITION_H
