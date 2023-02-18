#ifndef RD_CPP_CORE_LIFETIMEWRAPPER_H
#define RD_CPP_CORE_LIFETIMEWRAPPER_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "LifetimeImpl.h"

#include <std/hash.h>

#include <memory>

#include <rd_core_export.h>

namespace rd
{
	class Lifetime;

	template <>
	struct RD_CORE_API hash<Lifetime>
	{
		size_t operator()(const Lifetime& value) const noexcept;
	};

	class RD_CORE_API Lifetime final
	{
	private:
		using Allocator = std::allocator<LifetimeImpl>;

		static /*thread_local */ Allocator allocator;

		friend class LifetimeDefinition;

		friend struct hash<Lifetime>;

		std::shared_ptr<LifetimeImpl> ptr;

	public:
		static const Lifetime& Eternal();

		// region ctor/dtor

		Lifetime(const Lifetime& other) = default;

		Lifetime& operator=(const Lifetime& other) = default;

		Lifetime(Lifetime&& other) noexcept = default;

		Lifetime& operator=(Lifetime&& other) noexcept = default;

		~Lifetime() = default;
		// endregion

		friend bool RD_CORE_API operator==(const Lifetime& lw1, const Lifetime& lw2);
		friend bool RD_CORE_API operator!=(const Lifetime& lw1, const Lifetime& lw2);

		explicit Lifetime(bool is_eternal = false);

		LifetimeImpl* operator->() const;

		Lifetime create_nested() const;
	};

	inline size_t hash<Lifetime>::operator()(const Lifetime& value) const noexcept
	{
		return hash<std::shared_ptr<LifetimeImpl>>()(value.ptr);
	}
} // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_CORE_LIFETIMEWRAPPER_H
