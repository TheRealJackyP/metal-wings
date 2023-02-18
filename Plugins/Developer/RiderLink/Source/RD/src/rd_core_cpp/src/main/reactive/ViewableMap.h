#ifndef RD_CPP_CORE_VIEWABLE_MAP_H
#define RD_CPP_CORE_VIEWABLE_MAP_H

#include "base/IViewableMap.h"
#include "reactive/base/SignalX.h"

#include <util/core_util.h>
#include <std/unordered_map.h>

#include <thirdparty.hpp>

#include <iterator>
#include <utility>

namespace rd
{
	/**
	 * \brief complete class which has @code IViewableMap<K, V>'s properties
	 */
	template <typename K, typename V, typename KA = std::allocator<K>, typename VA =
	          std::allocator<V>>
	class ViewableMap : public IViewableMap<K, V>
	{
	public:
		using Event = typename IViewableMap<K, V>::Event;

	private:
		using WK = typename IViewableMap<K, V>::WK;
		using WV = typename IViewableMap<K, V>::WV;
		using OV = typename IViewableMap<K, V>::OV;
		using PA = typename std::allocator_traits<VA>::template rebind_alloc<std::pair<
			Wrapper<K>, Wrapper<V>>>;

		Signal<Event> change;

		using data_t = ordered_map<Wrapper<K>, Wrapper<V>, wrapper::TransparentHash<K>,
		                           wrapper::TransparentKeyEqual<K>, PA>;
		mutable data_t map;

	public:
		// region ctor/dtor

		ViewableMap() = default;

		ViewableMap(ViewableMap&&) = default;

		ViewableMap& operator=(ViewableMap&&) = default;

		virtual ~ViewableMap() override = default;
		// endregion

		// region iterators

	public:
		class iterator
		{
			friend class ViewableMap<K, V>;

			mutable typename data_t::iterator it_;

			explicit iterator(const typename data_t::iterator& it) : it_(it)
			{
			}

		public:
			using iterator_category = typename data_t::iterator::iterator_category;
			using key_type = K;
			using value_type = V;
			using difference_type = std::ptrdiff_t;
			using reference = const V&;
			using pointer = const V*;

			iterator(const iterator& other) = default;

			iterator(iterator&& other) noexcept = default;

			iterator& operator=(const iterator& other) = default;

			iterator& operator=(iterator&& other) noexcept = default;

			iterator& operator++()
			{
				++it_;
				return *this;
			}

			iterator operator++(int)
			{
				auto it = *this;
				++*this;
				return it;
			}

			iterator& operator--()
			{
				--it_;
				return *this;
			}

			iterator operator--(int)
			{
				auto it = *this;
				--*this;
				return it;
			}

			iterator& operator+=(difference_type delta)
			{
				it_ += delta;
				return *this;
			}

			iterator& operator-=(difference_type delta)
			{
				it_ -= delta;
				return *this;
			}

			iterator operator+(difference_type delta) const
			{
				auto it = *this;
				return it += delta;
			}

			iterator operator-(difference_type delta) const
			{
				auto it = *this;
				return it -= delta;
			}

			difference_type operator-(const iterator& other) const
			{
				return it_ - other.it_;
			}

			bool operator<(const iterator& other) const noexcept
			{
				return this->it_ < other.it_;
			}

			bool operator>(const iterator& other) const noexcept
			{
				return this->it_ > other.it_;
			}

			bool operator==(const iterator& other) const noexcept
			{
				return this->it_ == other.it_;
			}

			bool operator!=(const iterator& other) const noexcept
			{
				return !(*this == other);
			}

			bool operator<=(const iterator& other) const noexcept
			{
				return (this->it_ < other.it_) || (*this == other);
			}

			bool operator>=(const iterator& other) const noexcept
			{
				return (this->it_ > other.it_) || (*this == other);
			}

			reference operator*() const noexcept
			{
				return *it_.value();
			}

			pointer operator->() const noexcept
			{
				return it_.value().get();
			}

			const key_type& key() const
			{
				return *it_.key();
			}

			const value_type& value() const
			{
				return *it_.value();
			}
		};

		class reverse_iterator : public std::reverse_iterator<iterator>
		{
			using base_t = std::reverse_iterator<iterator>;

		public:
			using iterator_category = typename iterator::iterator_category;
			using key_type = typename iterator::key_type;
			using value_type = typename iterator::value_type;
			using difference_type = typename iterator::difference_type;
			using reference = typename iterator::reference;
			using pointer = typename iterator::pointer;

			reverse_iterator(const reverse_iterator& other) = default;

			reverse_iterator& operator=(const reverse_iterator& other) = default;

			explicit reverse_iterator(const iterator& other) : base_t(other)
			{
			};

			reverse_iterator& operator=(const iterator& other)
			{
				static_cast<base_t>(*this) = other;
			};

			const key_type& key() const
			{
				auto it = base_t::current;
				return (--(it)).key();
			}

			const value_type& value() const
			{
				auto it = base_t::current;
				return (--it).value();
			}
		};

		iterator begin() const
		{
			return iterator(map.begin());
		}

		iterator end() const
		{
			return iterator(map.end());
		}

		reverse_iterator rbegin() const
		{
			return reverse_iterator(end());
		}

		reverse_iterator rend() const
		{
			return reverse_iterator(begin());
		}

		// endregion

		virtual void advise(Lifetime lifetime,
		                    std::function<void(const Event&)> handler) const override
		{
			change.advise(lifetime, handler);
			/*for (auto const &[key, value] : map) {*/
			for (const auto& it : map)
			{
				auto& key = it.first;
				auto& value = it.second;
				handler(Event(typename Event::Add(&(*key), &(*value))));
			}
		}

		virtual const V* get(const K& key) const override
		{
			auto it = map.find(key);
			if (it == map.end())
			{
				return nullptr;
			}
			return &(*it->second);
		}

		virtual const V* set(WK key, WV value) const override
		{
			if (map.count(key) == 0)
			{
				/*auto[it, success] = map.emplace(std::make_unique<K>(std::move(key)), std::make_unique<V>(std::move(value)));*/
				auto node = map.emplace(std::move(key), std::move(value));
				auto& it = node.first;
				const auto& key_ptr = it->first;
				const auto& value_ptr = it->second;
				change.fire(typename Event::Add(&(*key_ptr), &(*value_ptr)));
				return nullptr;
			}
			auto it = map.find(key);
			const auto& key_ptr = it->first;
			const auto& value_ptr = it->second;

			if (*value_ptr != wrapper::get<V>(value))
			{
				// TO-DO more effective
				Wrapper<V> old_value = std::move(map.at(key));

				map.at(key_ptr) = Wrapper<V>(std::move(value));
				change.fire(
					typename Event::Update(&(*key_ptr), &(*old_value), &(*value_ptr)));
			}
			return &*(value_ptr);
		}

		virtual OV remove(const K& key) const override
		{
			if (map.count(key) > 0)
			{
				Wrapper<V> old_value = std::move(map.at(key));
				change.fire(typename Event::Remove(&key, &(*old_value)));
				map.erase(key);
				return wrapper::unwrap<V>(std::move(old_value));
			}
			return nullopt;
		}

		virtual void clear() const override
		{
			std::vector<Event> changes;
			/*for (auto const &[key, value] : map) {*/
			for (const auto& it : map)
			{
				changes.push_back(typename Event::Remove(&(*it.first), &(*it.second)));
			}
			for (const auto& it : changes)
			{
				change.fire(it);
			}
			map.clear();
		}

		virtual size_t size() const override
		{
			return map.size();
		}

		virtual bool empty() const override
		{
			return map.empty();
		}
	};
} // namespace rd

static_assert(std::is_move_constructible_v<rd::ViewableMap<int, int>>,
	"Is move constructible from ViewableMap<int, int>");

#endif	  // RD_CPP_CORE_VIEWABLE_MAP_H
