#ifndef RD_CPP_ANY_H
#define RD_CPP_ANY_H

#include "util/core_util.h"
#include "types/wrapper.h"
#include "serialization/ISerializable.h"

#include "thirdparty.hpp"

#include <memory>
#include <string>
#include <cstring>

namespace rd
{
	namespace any
	{
		using super_t = IPolymorphicSerializable;
		using wrapped_super_t = Wrapper<super_t>;
		using string = Wrapper<std::wstring>;
	} // namespace any
	/**
	 * \brief Presents union type to be interned. It may be either \link std::wstring,
	 * either \link rd::IPolymorphicSerializable.
	 */
	using InternedAny = variant<any::wrapped_super_t, any::string>;

	namespace any
	{
		template <typename T>
		InternedAny make_interned_any(Wrapper<T> wrapper)
		{
			return {wrapped_super_t(wrapper)};
		}

		template <>
		inline InternedAny make_interned_any<std::wstring>(
			Wrapper<std::wstring> wrapper)
		{
			return {wrapper};
		}

		template <typename T, typename Any>
		std::enable_if_t<!util::is_base_of_v<IPolymorphicSerializable, T>, string> get(
			const Any& any)
		{
			return get<string>(any);
		}

		template <typename T, typename Any>
		std::enable_if_t<util::is_base_of_v<IPolymorphicSerializable, T>, Wrapper<T>>
		get(Any&& any)
		{
			return Wrapper<T>::dynamic(get<wrapped_super_t>(std::forward<Any>(any)));
		}

		struct TransparentKeyEqual
		{
			using is_transparent = void;

			bool operator()(const InternedAny& val_l, const InternedAny& val_r) const
			{
				return val_l == val_r;
			}

			bool operator()(const InternedAny& val_l,
			                const wrapped_super_t& val_r) const
			{
				return visit(util::make_visitor(
					             [&](const wrapped_super_t& value)
					             {
						             return *value == *val_r;
					             }, [](
					             const string&)
					             {
						             return false;
					             }),
				             val_l);
			}

			bool operator()(const super_t& val_l, const InternedAny& val_r) const
			{
				return operator()(val_r, val_l);
			}

			bool operator()(const InternedAny& val_l, const super_t& val_r) const
			{
				return visit(util::make_visitor(
					             [&](const wrapped_super_t& value)
					             {
						             return *value == val_r;
					             }, [](
					             const string&)
					             {
						             return false;
					             }),
				             val_l);
			}

			bool operator()(const wrapped_super_t& val_l,
			                const InternedAny& val_r) const
			{
				return operator()(val_r, val_l);
			}

			bool operator()(const InternedAny& val_l, const string& val_r) const
			{
				return visit(util::make_visitor(
					             [](const wrapped_super_t& /*value*/) { return false; },
					             [&](
					             const string& s)
					             {
						             return s == val_r;
					             }),
				             val_l);
			}

			bool operator()(const string& val_l, const InternedAny& val_r) const
			{
				return operator()(val_r, val_l);
			}
		};

		struct TransparentHash
		{
			using is_transparent = void;
			using transparent_key_equal = std::equal_to<>;

			size_t operator()(const InternedAny& value) const noexcept
			{
				return visit(util::make_visitor(
					             [](const wrapped_super_t& value)
					             {
						             return rd::hash<wrapped_super_t>()(value);
					             },
					             [](const string& value)
					             {
						             return rd::hash<string>()(value);
					             }),
				             value);
			}

			size_t operator()(const wrapped_super_t& value) const noexcept
			{
				return rd::hash<wrapped_super_t>()(value);
			}

			size_t operator()(const super_t& value) const noexcept
			{
				return rd::hash<super_t>()(value);
			}

			size_t operator()(const string& value) const noexcept
			{
				return rd::hash<string>()(value);
			}
		};
	} // namespace any
} // namespace rd

static_assert(!std::is_trivially_copy_constructible_v<rd::Wrapper<
		rd::IPolymorphicSerializable>>,
	"wrapper mustn't be trivially_copy_constructible");

#endif	  // RD_CPP_ANY_H
