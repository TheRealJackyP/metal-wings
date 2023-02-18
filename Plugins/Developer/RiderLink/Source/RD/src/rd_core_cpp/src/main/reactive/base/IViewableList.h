#ifndef RD_CPP_IVIEWABLELIST_H
#define RD_CPP_IVIEWABLELIST_H

#include "interfaces.h"
#include "viewable_collections.h"

#include <lifetime/LifetimeDefinition.h>
#include <util/overloaded.h>
#include <types/wrapper.h>

#include <std/unordered_map.h>

#include <vector>
#include <utility>
#include <algorithm>

#include "thirdparty.hpp"

namespace rd
{
	namespace detail
	{
		template <typename T>
		class ListEvent
		{
		public:
			class Add
			{
			public:
				int32_t index;
				const T* new_value;

				Add(int32_t index, const T* new_value) : index(index),
				                                         new_value(new_value)
				{
				}
			};

			class Update
			{
			public:
				int32_t index;
				const T* old_value;
				const T* new_value;

				Update(int32_t index, const T* old_value, const T* new_value) :
					index(index), old_value(old_value), new_value(new_value)
				{
				}
			};

			class Remove
			{
			public:
				int32_t index;
				const T* old_value;

				Remove(int32_t index, const T* old_value) : index(index),
					old_value(old_value)
				{
				}
			};

			variant<Add, Update, Remove> v;

			ListEvent(Add x) : v(x)
			{
			}

			ListEvent(Update x) : v(x)
			{
			}

			ListEvent(Remove x) : v(x)
			{
			}

			int32_t get_index() const
			{
				return visit(util::make_visitor([](const Add& e) { return e.index; },
				                                [](const Update& e) { return e.index; },
				                                [](const Remove& e)
				                                {
					                                return e.index;
				                                }),
				             v);
			}

			const T* get_new_value() const
			{
				return visit(util::make_visitor(
					             [](const Add& e) { return e.new_value; },
					             [](const Update& e) { return e.new_value; },
					             [](const Remove& /*e*/)
					             {
						             return static_cast<const T*>(nullptr);
					             }),
				             v);
			}

			friend std::string to_string(const ListEvent& e)
			{
				std::string res = visit(
					util::make_visitor(
						[](const Add& e)
						{
							return "Add " + std::to_string(e.index) + ":" + to_string(
								*e.new_value);
						},
						[](const Update& e)
						{
							return "Update " + std::to_string(e.index) + ":" +
								//                       to_string(e.old_value) + ":" +
								to_string(*e.new_value);
						},
						[](const Remove& e)
						{
							return "Remove " + std::to_string(e.index);
						}),
					e.v);
				return res;
			}
		};
	} // namespace detail
	/**
	 * \brief A list allowing its contents to be observed.
	 * \tparam T type of stored values (may be abstract)
	 */
	template <typename T>
	class IViewableList : public IViewable<std::pair<size_t, const T*>>,
	                      public ISource<detail::ListEvent<T>>
	{
	protected:
		using WT = value_or_wrapper<T>;

	public:
		/**
		 * \brief Represents an addition, update or removal of an element in the list.
		 */
		using Event = detail::ListEvent<T>;

	protected:
		mutable unordered_map<Lifetime, std::vector<LifetimeDefinition>> lifetimes;

	public:
		// region ctor/dtor

		IViewableList() = default;

		IViewableList(IViewableList&&) = default;

		IViewableList& operator=(IViewableList&&) = default;

		virtual ~IViewableList() override = default;
		// endregion

		/**
		 * \brief Adds a subscription to additions and removals of list elements. When a list element is updated,
		 * the [handler] is called twice: to report the removal of the old element and the addition of the new one.
		 * \param lifetime lifetime of subscription.
		 * \param handler to be called.
		 */
		void advise_add_remove(Lifetime lifetime,
		                       std::function<void(AddRemove, size_t, const T&)> handler)
		const
		{
			advise(lifetime, [handler](Event e)
			{
				visit(util::make_visitor([handler](const typename Event::Add& e)
				                         {
					                         handler(AddRemove::ADD, e.index,
					                                 *e.new_value);
				                         },
				                         [handler](const typename Event::Update& e)
				                         {
					                         handler(AddRemove::REMOVE, e.index,
					                                 *e.old_value);
					                         handler(AddRemove::ADD, e.index,
					                                 *e.new_value);
				                         },
				                         [handler](const typename Event::Remove& e)
				                         {
					                         handler(AddRemove::REMOVE, e.index,
					                                 *e.old_value);
				                         }),
				      e.v);
			});
		}

		/**
		 * \brief Adds a subscription to changes of the contents of the list.
		 * \param lifetime lifetime of subscription.
		 * \param handler to be called.
		 */
		virtual void view(Lifetime lifetime,
		                  std::function<void(Lifetime lifetime,
		                                     const std::pair<size_t, const T*>&)>
		                  handler) const override
		{
			view(lifetime, [handler](Lifetime lt, size_t idx, const T& v)
			{
				handler(lt, std::make_pair(idx, &v));
			});
		}

		/**
		 * \brief @see view	above
		 */
		virtual void view(Lifetime lifetime,
		                  std::function<void(Lifetime, size_t,
		                                     const T&)> handler) const
		{
			advise_add_remove(lifetime,
			                  [this, lifetime, handler](
			                  AddRemove kind, size_t idx, const T& value)
			                  {
				                  switch (kind)
				                  {
				                  case AddRemove::ADD:
					                  {
						                  LifetimeDefinition def(lifetime);
						                  std::vector<LifetimeDefinition>& v = lifetimes
							                  [lifetime];
						                  auto it = v.emplace(
							                  v.begin() + idx, std::move(def));
						                  handler(it->lifetime, idx, value);
						                  break;
					                  }
				                  case AddRemove::REMOVE:
					                  {
						                  LifetimeDefinition def = std::move(
							                  lifetimes.at(lifetime)[idx]);
						                  std::vector<LifetimeDefinition>& v = lifetimes
							                  .at(lifetime);
						                  v.erase(v.begin() + idx);
						                  def.terminate();
						                  break;
					                  }
				                  }
			                  });
		}

		virtual void advise(Lifetime lifetime,
		                    std::function<void(const Event&)> handler) const override =
		0;

		virtual bool add(WT element) const = 0;

		virtual bool add(size_t index, WT element) const = 0;

		virtual WT removeAt(size_t index) const = 0;

		virtual bool remove(const T& element) const = 0;

		virtual const T& get(size_t index) const = 0;

		virtual WT set(size_t index, WT element) const = 0;

		virtual bool addAll(size_t index, std::vector<WT> elements) const = 0;

		virtual bool addAll(std::vector<WT> elements) const = 0;

		virtual void clear() const = 0;

		virtual bool removeAll(std::vector<WT> elements) const = 0;

		virtual size_t size() const = 0;

		virtual bool empty() const = 0;

		template <typename... Args>
		bool emplace_add(Args&&... args) const
		{
			return add(WT{std::forward<Args>(args)...});
		}

		template <typename... Args>
		bool emplace_add(size_t index, Args&&... args) const
		{
			return add(index, WT{std::forward<Args>(args)...});
		}

		template <typename... Args>
		WT emplace_set(size_t index, Args&&... args) const
		{
			return set(index, WT{std::forward<Args>(args)...});
		}

		template <typename U>
		friend std::enable_if_t<(!std::is_abstract_v<U>), std::vector<U>>
		convert_to_list(
			const IViewableList<U>& list);

	protected:
		virtual const std::vector<Wrapper<T>>& getList() const = 0;
	};

	template <typename T>
	std::enable_if_t<(!std::is_abstract_v<T>), std::vector<T>> convert_to_list(
		const IViewableList<T>& list)
	{
		std::vector<T> res(list.size());
		std::transform(list.getList().begin(), list.getList().end(), res.begin(),
		               [](const Wrapper<T>& ptr) { return *ptr; });
		return res;
	}
} // namespace rd

static_assert(
	std::is_move_constructible_v<rd::IViewableList<int>::Event>,
	"Is move constructible from IViewableList<int>::Event");

#endif	  // RD_CPP_IVIEWABLELIST_H
