#ifndef RD_CPP_POLYMORPHIC_H
#define RD_CPP_POLYMORPHIC_H

#include "protocol/Buffer.h"
#include "base/RdReactiveBase.h"

#include <type_traits>

namespace rd
{
	// region predeclared

	class SerializationCtx;
	// endregion

	/**
	 * \brief Maintains "SerDes" for statically polymorphic type [T].
	 * Requires static "read" and "write" methods as in common case below.
	 * \tparam T type to "SerDes"
	 * \tparam R trait specialisation (void by default)
	 */
	template <typename T, typename R = void>
	class Polymorphic
	{
	public:
		static T read(SerializationCtx& ctx, Buffer& buffer)
		{
			return T::read(ctx, buffer);
		}

		static void write(SerializationCtx& ctx, Buffer& buffer, const T& value)
		{
			value.write(ctx, buffer);
		}

		static void write(SerializationCtx& ctx, Buffer& buffer,
		                  const Wrapper<T>& value)
		{
			value->write(ctx, buffer);
		}
	};

	template <typename T>
	class Polymorphic<T, std::enable_if_t<std::is_integral_v<T>>>
	{
	public:
		static T read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_integral<T>();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer, const T& value)
		{
			buffer.write_integral<T>(value);
		}
	};

	template <typename T>
	class Polymorphic<T, std::enable_if_t<std::is_floating_point_v<T>>>
	{
	public:
		static T read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_floating_point<T>();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer, const T& value)
		{
			buffer.write_floating_point<T>(value);
		}
	};

	// class Polymorphic<int, void>;

	template <template <class, class> class C, typename T, typename A>
	class Polymorphic<C<T, A>, std::enable_if_t<!util::is_base_of_v<RdReactiveBase, T>
		                  &&
		                  !util::is_same_v<Wrapper<T, A>, C<T, A>>>>
	{
	public:
		static C<T, A> read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_array<C, T, A>();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer,
		                  const C<T, A>& value)
		{
			buffer.write_array<C, T, A>(value);
		}
	};

	template <>
	class Polymorphic<bool>
	{
	public:
		static bool read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_bool();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer, const bool& value)
		{
			buffer.write_bool(value);
		}
	};

	template <>
	class Polymorphic<wchar_t>
	{
	public:
		static wchar_t read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_char();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer, const bool& value)
		{
			buffer.write_char(value);
		}
	};

	template <>
	class Polymorphic<std::wstring>
	{
	public:
		static std::wstring read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_wstring();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer,
		                  const std::wstring& value)
		{
			buffer.write_wstring(value);
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer,
		                  const Wrapper<std::wstring>& value)
		{
			buffer.write_wstring(*value);
		}
	};

	template <>
	class Polymorphic<DateTime>
	{
	public:
		static DateTime read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_date_time();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer,
		                  const DateTime& value)
		{
			buffer.write_date_time(value);
		}
	};

	template <>
	class Polymorphic<Void>
	{
	public:
		static Void read(SerializationCtx& /*ctx*/, Buffer& /*buffer*/)
		{
			return {};
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& /*buffer*/,
		                  const Void& /*value*/)
		{
		}
	};

	template <typename T>
	class Polymorphic<T, std::enable_if_t<util::is_base_of_v<RdReactiveBase, T>>>
	{
	public:
		static T read(SerializationCtx& ctx, Buffer& buffer)
		{
			return T::read(ctx, buffer);
		}

		static void write(SerializationCtx& ctx, Buffer& buffer, const T& value)
		{
			value.write(ctx, buffer);
		}
	};

	template <typename T>
	class Polymorphic<T, std::enable_if_t<util::is_enum_v<T>>>
	{
	public:
		static T read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			return buffer.read_enum<T>();
		}

		static void write(SerializationCtx& /*ctx*/, Buffer& buffer, const T& value)
		{
			buffer.write_enum<T>(value);
		}
	};

	template <typename T>
	class Polymorphic<optional<T>>
	{
	public:
		static optional<T> read(SerializationCtx& ctx, Buffer& buffer)
		{
			return buffer.read_nullable<T>([&ctx, &buffer]()
			{
				return Polymorphic<T>::read(ctx, buffer);
			});
		}

		static void write(SerializationCtx& ctx, Buffer& buffer,
		                  const optional<T>& value)
		{
			buffer.write_nullable<T>(value, [&ctx, &buffer](const T& v)
			{
				Polymorphic<T>::write(ctx, buffer, v);
			});
		}
	};

	template <typename T, typename A>
	class Polymorphic<Wrapper<T, A>>
	{
	public:
		static void write(SerializationCtx& ctx, Buffer& buffer,
		                  const Wrapper<T, A>& value)
		{
			value->write(ctx, buffer);
		}
	};
} // namespace rd

#endif	  // RD_CPP_POLYMORPHIC_H
