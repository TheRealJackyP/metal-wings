#ifndef RD_CPP_IVIEWABLEMAP_H
#define RD_CPP_IVIEWABLEMAP_H

#include "lifetime/LifetimeDefinition.h"
#include "util/overloaded.h"
#include "interfaces.h"
#include "viewable_collections.h"
#include "util/core_util.h"

#include "std/unordered_map.h"

#include "thirdparty.hpp"

namespace rd
{
	namespace detail
	{
		template <typename K, typename V>
		class MapEvent
		{
		public:
			class Add
			{
			public:
				const K* key;
				const V* new_value;

				Add(const K* key, const V* new_value) : key(key), new_value(new_value)
				{
				}
			};

			class Update
			{
			public:
				const K* key;
				const V* old_value;
				const V* new_value;

				Update(const K* key, const V* old_value, const V* new_value) : key(key),
					old_value(old_value), new_value(new_value)
				{
				}
			};

			class Remove
			{
			public:
				const K* key;
				const V* old_value;

				Remove(const K* key, const V* old_value) : key(key),
					old_value(old_value)
				{
				}
			};

			variant<Add, Update, Remove> v;

			MapEvent(Add x) : v(x)
			{
			}

			MapEvent(Update x) : v(x)
			{
			}

			MapEvent(Remove x) : v(x)
			{
			}

			const K* get_key() const
			{
				return visit(
					util::make_visitor([](const Add& e) { return e.key; },
					                   [](const Update& e) { return e.key; },
					                   [](const Remove& e) { return e.key; }),
					v);
			}

			const V* get_old_value() const
			{
				return visit(util::make_visitor(
					             [](const Add& e)
					             {
						             return static_cast<const V*>(nullptr);
					             },
					             [](const Update& e) { return e.old_value; },
					             [](const Remove& e) { e.old_value; }),
				             v);
			}

			const V* get_new_value() const
			{
				return visit(util::make_visitor(
					             [](const Add& e) { return e.new_value; },
					             [](const Update& e) { return e.new_value; },
					             [](const Remove& /*e*/)
					             {
						             return static_cast<const V*>(nullptr);
					             }),
				             v);
			}

			friend std::string to_string(const MapEvent& e)
			{
				std::string res =
					visit(util::make_visitor([](const Add& e)
				                         -> std::string
					                         {
						                         return "Add " + to_string(*e.key) + ":"
							                         + to_string(*e.new_value);
					                         },
					                         [](const Update& e) -> std::string
					                         {
						                         return "Update " + to_string(*e.key) +
							                         ":" +
							                         //                       to_string(e.old_value) + ":" +
							                         to_string(*e.new_value);
					                         },
					                         [](const Remove& e) -> std::string
					                         {
						                         return "Remove " + to_string(*e.key);
					                         }),
					      e.v);
				return res;
			}
		};
	} // namespace detail

	/**
	 * \brief A set allowing its contents to be observed.
	 * \tparam K type of stored keys (may be abstract)
	 * \tparam V type of stored values (may be abstract)
	 */
	template <typename K, typename V>
	class IViewableMap : public IViewable<std::pair<const K*, const V*>>,
	                     public ISource<detail::MapEvent<K, V>>
	{
	protected:
		using WK = value_or_wrapper<K>;
		using WV = value_or_wrapper<V>;
		using OV = opt_or_wrapper<V>;

		mutable unordered_map<Lifetime,
		                      ordered_map<
			                      const K*, LifetimeDefinition, wrapper::TransparentHash
			                      <K>,
			                      wrapper::TransparentKeyEqual<K>>>
		lifetimes;

	public:
		/**
		 * \brief Represents an addition, update or removal of an element in the map.
		 */
		using Event = detail::MapEvent<K, V>;

		// region ctor/dtor

		IViewableMap() = default;

		IViewableMap(IViewableMap&&) = default;

		IViewableMap& operator=(IViewableMap&&) = default;

		virtual ~IViewableMap() override = default;
		// endregion

		virtual void view(Lifetime lifetime,
		                  std::function<void(Lifetime lifetime,
		                                     const std::pair<const K*, const V*>&)

		                  >
		                  handler) const override
		{
			advise_add_remove(lifetime,
			                  [this, lifetime, handler](
			                  AddRemove kind, const K& key, const V& value)
			                  {
				                  const std::pair<const K*, const V*> entry =
					                  std::make_pair(&key, &value);
				                  switch (kind)
				                  {
				                  case AddRemove::ADD:
					                  {
						                  if (lifetimes[lifetime].count(key) == 0)
						                  {
							                  /*auto const &[it, inserted] = lifetimes[lifetime].emplace(key, LifetimeDefinition(lifetime));*/
							                  const auto& pair = lifetimes[lifetime].
								                  emplace(
									                  &key,
									                  LifetimeDefinition(lifetime));
							                  auto& it = pair.first;
							                  auto& inserted = pair.second;
							                  RD_ASSERT_MSG(
								                  inserted,
								                  "lifetime definition already exists in viewable map by key:"
								                  + to_string(key));
							                  handler(it->second.lifetime, entry);
						                  }
						                  break;
					                  }
				                  case AddRemove::REMOVE:
					                  {
						                  RD_ASSERT_MSG(
							                  lifetimes.at(lifetime).count(key) > 0,
							                  "attempting to remove non-existing lifetime in viewable map by key:"
							                  + to_string(key));
						                  LifetimeDefinition def = std::move(
							                  lifetimes.at(lifetime).at(key));
						                  lifetimes.at(lifetime).erase(key);
						                  def.terminate();
						                  break;
					                  }
				                  }
			                  });
		}

		/**
		 * \brief Adds a subscription to additions and removals of map elements. When a map element is updated, the [handler]
		 * is called twice: to report the removal of the old element and the addition of the new one.
		 * \param lifetime lifetime of subscription.
		 * \param handler to be called.
		 */
		void advise_add_remove(Lifetime lifetime,
		                       std::function<void(AddRemove, const K&, const V&)

		                       >
		                       handler) const
		{
			advise(lifetime, [handler](Event e)
			{
				visit(util::make_visitor([handler](const typename Event::Add& e)
				                         {
					                         handler(AddRemove::ADD, *e.key,
					                                 *e.new_value);
				                         },
				                         [handler](const typename Event::Update& e)
				                         {
					                         handler(AddRemove::REMOVE, *e.key,
					                                 *e.old_value);
					                         handler(AddRemove::ADD, *e.key,
					                                 *e.new_value);
				                         },
				                         [handler](const typename Event::Remove& e)
				                         {
					                         handler(AddRemove::REMOVE, *e.key,
					                                 *e.old_value);
				                         }),
				      e.v);
			});
		}

		/**
		 * \brief Adds a subscription to changes of the contents of the map, with the handler receiving keys and values
		 * as separate parameters.
		 *
		 * \details When [handler] is initially added, it is called receiving all keys and values currently in the map.
		 * Every time a key/value pair is added to the map, the [handler] is called receiving the new key and value.
		 * The [Lifetime] instance passed to the handler expires when the key/value pair is removed from the map.
		 *
		 * \param lifetime lifetime of subscription.
		 * \param handler to be called.
		 */
		virtual void view(Lifetime lifetime,
		                  std::function<void(Lifetime, const K&, const V&)

		                  >
		                  handler) const
		{
			view(lifetime,
			     [handler](Lifetime lf, const std::pair<const K*, const V*> entry)
			     {
				     handler(lf, *entry.first, *entry.second);
			     });
		}

		virtual void advise(Lifetime lifetime,
		                    std::function<void(const Event&)> handler) const override =
		0;

		virtual const V* get(const K&) const = 0;

		virtual const V* set(WK, WV) const = 0;

		virtual OV remove(const K&) const = 0;

		virtual void clear() const = 0;

		virtual size_t size() const = 0;

		virtual bool empty() const = 0;

		template <typename... Args>
		const V* emplace_set(WK wk, Args&&... args) const
		{
			return set(std::move(wk), WV{std::forward<Args>(args)...});
		}
	};
} // namespace rd

static_assert(std::is_move_constructible_v<rd::IViewableMap<int, int>::Event>,
	"Is move constructible from IViewableMap<int, int>::Event");

#endif	  // RD_CPP_IVIEWABLEMAP_H
