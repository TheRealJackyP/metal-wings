#ifndef RD_CPP_RDPROPERTY_H
#define RD_CPP_RDPROPERTY_H

#include "base/RdPropertyBase.h"
#include "serialization/Polymorphic.h"
#include "serialization/ISerializable.h"
#include "std/allocator.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace rd
{
	/**
	 * \brief Reactive property for connection through wire.
	
	 * \tparam T type of stored value
	 * \tparam S "SerDes" for value
	 */
	template <typename T, typename S = Polymorphic<T>, typename A = allocator<T>>
	class RdProperty final : public RdPropertyBase<T, S>, public ISerializable
	{
	public:
		using value_type = T;

		// region ctor/dtor

		RdProperty() = default;

		RdProperty(const RdProperty&) = delete;

		RdProperty& operator=(const RdProperty&) = delete;

		RdProperty(RdProperty&&) = default;

		RdProperty& operator=(RdProperty&&) = default;

		template <typename F>
		explicit RdProperty(F&& value) : RdPropertyBase<T, S>(std::forward<F>(value))
		{
		}

		virtual ~RdProperty() override = default;
		// endregion

		static RdProperty<T, S> read(SerializationCtx& ctx, Buffer& buffer)
		{
			RdId id = RdId::read(buffer);
			bool not_null = buffer.read_bool(); // not null/
			(void)not_null;
			auto value = S::read(ctx, buffer);
			RdProperty<T, S> property;
			property.value = std::move(value);
			withId(property, id);
			return property;
		}

		virtual void write(SerializationCtx& ctx, Buffer& buffer) const override
		{
			this->rdid.write(buffer);
			buffer.write_bool(true);
			S::write(ctx, buffer, this->get());
		}

		virtual void advise(Lifetime lifetime,
		                    std::function<void(const T&)> handler) const override
		{
			RdPropertyBase<T, S>::advise(lifetime, std::move(handler));
		}

		RdProperty<T, S>& slave()
		{
			this->is_master = false;
			return *this;
		}

		virtual void
		identify(const Identities& identities, const RdId& id) const override
		{
			RdBindableBase::identify(identities, id);
			if (!this->optimize_nested && this->has_value())
			{
				identifyPolymorphic(this->get(), identities, identities.next(id));
			}
		}

		friend bool operator==(const RdProperty& lhs, const RdProperty& rhs)
		{
			return &lhs == &rhs;
		}

		friend bool operator!=(const RdProperty& lhs, const RdProperty& rhs)
		{
			return !(rhs == lhs);
		}

		friend std::string to_string(const RdProperty& value)
		{
			return to_string(static_cast<const Property<T>&>(value));
		}
	};
} // namespace rd

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static_assert(std::is_move_constructible_v<rd::RdProperty<int>>,
	"Is move constructible from RdProperty<int>");

#endif	  // RD_CPP_RDPROPERTY_H
