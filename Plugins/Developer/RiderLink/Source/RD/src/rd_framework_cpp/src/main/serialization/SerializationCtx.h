#ifndef RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H
#define RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "protocol/Buffer.h"
#include "protocol/RdId.h"

#include "std/unordered_map.h"

#include <functional>
#include <string>
#include <utility>
#include <regex>

#include <rd_framework_export.h>

namespace rd
{
	// region predeclared

	class IProtocol;

	class Serializers;

	class InternRoot;

	class RdBindableBase;
	// endregion

	class RD_FRAMEWORK_API SerializationCtx
	{
		const Serializers* serializers = nullptr;

	public:
		using roots_t = unordered_map<util::hash_t, const InternRoot*>;

		roots_t intern_roots{};

		// region ctor/dtor

		//    SerializationCtx() = delete;

		SerializationCtx(const SerializationCtx& other) = delete;

		SerializationCtx& operator=(const SerializationCtx& other) = delete;

		SerializationCtx(SerializationCtx&& other) = default;

		SerializationCtx& operator=(SerializationCtx&& other) = default;

		//		explicit SerializationCtx(const Serializers *serializers = nullptr);

		explicit SerializationCtx(const Serializers* serializers,
		                          roots_t intern_roots = {});

		SerializationCtx withInternRootsHere(const RdBindableBase& owner,
		                                     std::initializer_list<std::string>
		                                     new_roots) const;

		// endregion

		template <typename T, util::hash_t InternKey>
		Wrapper<T> readInterned(Buffer& buffer,
		                        std::function<T(SerializationCtx&, Buffer&)>
		                        readValueDelegate);

		template <typename T, util::hash_t InternKey, typename F,
		          typename = std::enable_if_t<util::is_invocable<
			          F, SerializationCtx&, Buffer&, T>::value>>
		void writeInterned(Buffer& buffer, const Wrapper<T>& value,
		                   F&& writeValueDelegate);

		const Serializers& get_serializers() const;
	};
} // namespace rd

#include "intern/InternRoot.h"

namespace rd
{
	template <typename T, util::hash_t InternKey>
	Wrapper<T> SerializationCtx::readInterned(Buffer& buffer,
	                                          std::function<T(
		                                          SerializationCtx&, Buffer&)>
	                                          readValueDelegate)
	{
		auto it = intern_roots.find(InternKey);
		if (it != intern_roots.end())
		{
			int32_t index = buffer.read_integral<int32_t>() ^ 1;
			return it->second->un_intern_value<T>(index);
		}
		return wrapper::make_wrapper<T>(readValueDelegate(*this, buffer));
	}

	template <typename T, util::hash_t InternKey, typename F, typename>
	void SerializationCtx::writeInterned(Buffer& buffer, const Wrapper<T>& value,
	                                     F&& writeValueDelegate)
	{
		auto it = intern_roots.find(InternKey);
		if (it != intern_roots.end())
		{
			int32_t index = it->second->intern_value<T>(value);
			buffer.write_integral<int32_t>(index);
		}
		else
		{
			writeValueDelegate(const_cast<SerializationCtx&>(*this), buffer, *value);
		}
	}
} // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif	  // RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H
