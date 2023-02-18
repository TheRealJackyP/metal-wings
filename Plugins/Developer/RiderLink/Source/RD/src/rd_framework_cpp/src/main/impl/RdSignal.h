#ifndef RD_CPP_RDSIGNAL_H
#define RD_CPP_RDSIGNAL_H

#include "lifetime/Lifetime.h"
#include "reactive/base/interfaces.h"
#include "scheduler/base/IScheduler.h"
#include "reactive/base/SignalX.h"
#include "base/RdReactiveBase.h"
#include "serialization/Polymorphic.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace rd
{
	/**
	 * \brief Reactive signal for connection through wire.
	 *
	 * \tparam T type of events
	 * \tparam S "SerDes" for events
	 */
	template <typename T, typename S = Polymorphic<T>>
	class RdSignal final : public RdReactiveBase, public ISignal<T>,
	                       public ISerializable
	{
	private:
		using WT = typename ISignal<T>::WT;

		std::string logmsg(const T& value) const
		{
			return "signal " + to_string(location) + " " + to_string(rdid) +
				":: value = " + to_string(value);
		}

		mutable IScheduler* wire_scheduler{};

	private:
		void set_wire_scheduler(IScheduler* scheduler) const
		{
			wire_scheduler = scheduler;
		}

	protected:
		Signal<T> signal;

	public:
		// region ctor/dtor

		RdSignal(const RdSignal&) = delete;

		RdSignal& operator=(const RdSignal&) = delete;

		RdSignal() = default;

		RdSignal(RdSignal&&) = default;

		RdSignal& operator=(RdSignal&&) = default;

		virtual ~RdSignal() override = default;
		// endregion

		static RdSignal<T, S> read(SerializationCtx& /*ctx*/, Buffer& buffer)
		{
			RdSignal<T, S> res;
			const RdId& id = RdId::read(buffer);
			withId(res, id);
			return res;
		}

		virtual void write(SerializationCtx& /*ctx*/, Buffer& buffer) const override
		{
			rdid.write(buffer);
		}

		virtual void init(Lifetime lifetime) const override
		{
			RdReactiveBase::init(lifetime);
			set_wire_scheduler(get_default_scheduler());
			get_wire()->advise(lifetime, this);
		}

		virtual void on_wire_received(Buffer buffer) const override
		{
			auto value = S::read(this->get_serialization_context(), buffer);
			spdlog::get("logReceived")->trace("RECV{}", logmsg(wrapper::get<T>(value)));

			signal.fire(wrapper::get<T>(value));
		}

		using ISignal<T>::fire;

		virtual void fire(const T& value) const override
		{
			// Disabling checking for bounding, to fix async signal being called unbound on another thread.
			// Copying hack from rd-net
			// https://github.com/JetBrains/rd/blob/d00d07c38784af5743c75ceca4bbceb92f6f8149/rd-net/RdFramework/Impl/RdSignal.cs#L72-L95
			// assert_bound();
			if (!async)
			{
				assert_threading();
			}

			if (async && !is_bound())
			{
				return;
			}

			get_wire()->send(rdid, [this, &value](Buffer& buffer)
			{
				spdlog::get("logSend")->trace("SEND{}", logmsg(value));
				S::write(get_serialization_context(), buffer, value);
			});
			signal.fire(value);
		}

		using ISource<T>::advise;

		virtual void advise(Lifetime lifetime,
		                    std::function<void(const T&)> handler) const override
		{
			if (is_bound())
			{
				assert_threading();
			}
			signal.advise(lifetime, handler);
		}

		template <typename F>
		void advise_on(Lifetime lifetime, IScheduler* scheduler, F&& handler)
		{
			if (is_bound())
			{
				assert_threading();
			}
			set_wire_scheduler(scheduler);
			signal.advise(lifetime, std::forward<F>(handler));
		}

		virtual IScheduler* get_wire_scheduler() const override
		{
			return wire_scheduler;
		}

		friend std::string to_string(const RdSignal&)
		{
			return "";
		}
	};
} // namespace rd

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static_assert(std::is_move_constructible_v<rd::RdSignal<int>>,
	"Is not move constructible from RdSignal<int>");

#endif	  // RD_CPP_RDSIGNAL_H
