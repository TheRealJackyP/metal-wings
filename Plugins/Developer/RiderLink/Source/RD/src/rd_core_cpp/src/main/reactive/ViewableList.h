#ifndef RD_CPP_CORE_VIEWABLELIST_H
#define RD_CPP_CORE_VIEWABLELIST_H

#include "base/IViewableList.h"
#include "reactive/base/SignalX.h"
#include "util/core_util.h"

#include <algorithm>
#include <iterator>
#include <utility>

namespace rd
{
	/**
	 * \brief complete class which has @code IViewableList<T>'s properties
	 */
	template <typename T, typename A = allocator<T>>
	class ViewableList : public IViewableList<T>
	{
	public:
		using Event = typename IViewableList<T>::Event;

	private:
		using WA = typename std::allocator_traits<A>::template rebind_alloc<Wrapper<T>>;

		using data_t = std::vector<Wrapper<T>, WA>;
		mutable data_t list;
		Signal<Event> change;

	protected:
		using WT = typename IViewableList<T>::WT;

		virtual const std::vector<Wrapper<T>>& getList() const override
		{
			return list;
		}

	public:
		// region ctor/dtor

		ViewableList() = default;

		ViewableList(ViewableList&&) = default;

		ViewableList& operator=(ViewableList&&) = default;

		virtual ~ViewableList() override = default;

		// endregion

		// region iterators
	public:
		class iterator
		{
			friend class ViewableList<T>;

			typename data_t::iterator it_;

			explicit iterator(const typename data_t::iterator& it) : it_(it)
			{
			}

		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = const T*;
			using reference = const T&;

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

			reference operator*() noexcept
			{
				return **it_;
			}

			reference operator*() const noexcept
			{
				return **it_;
			}

			pointer operator->() noexcept
			{
				return (*it_).get();
			}

			pointer operator->() const noexcept
			{
				return (*it_).get();
			}
		};

		using reverse_iterator = std::reverse_iterator<iterator>;

		iterator begin() const
		{
			return iterator(list.begin());
		}

		iterator end() const
		{
			return iterator(list.end());
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
			if (lifetime->is_terminated())
			{
				return;
			}
			change.advise(lifetime, handler);
			for (int32_t i = 0; i < static_cast<int32_t>(size()); ++i)
			{
				handler(typename Event::Add(i, &(*list[i])));
			}
		}

		virtual bool add(WT element) const override
		{
			list.emplace_back(std::move(element));
			change.fire(
				typename Event::Add(static_cast<int32_t>(size()) - 1, &(*list.back())));
			return true;
		}

		virtual bool add(size_t index, WT element) const override
		{
			list.emplace(list.begin() + index, std::move(element));
			change.fire(
				typename Event::Add(static_cast<int32_t>(index), &(*list[index])));
			return true;
		}

		virtual WT removeAt(size_t index) const override
		{
			auto res = std::move(list[index]);
			list.erase(list.begin() + index);

			change.fire(typename Event::Remove(static_cast<int32_t>(index), &(*res)));
			return wrapper::unwrap<T>(std::move(res));
		}

		virtual bool remove(const T& element) const override
		{
			auto it = std::find_if(list.begin(), list.end(), [&element](const auto& p)
			{
				return *p == element;
			});
			if (it == list.end())
			{
				return false;
			}
			ViewableList::removeAt(std::distance(list.begin(), it));
			return true;
		}

		virtual const T& get(size_t index) const override
		{
			return *list[index];
		}

		virtual WT set(size_t index, WT element) const override
		{
			auto old_value = std::move(list[index]);
			list[index] = Wrapper<T>(std::move(element));
			change.fire(typename Event::Update(static_cast<int32_t>(index),
			                                   &(*old_value), &(*list[index]))); //???
			return wrapper::unwrap<T>(std::move(old_value));
		}

		virtual bool addAll(size_t index, std::vector<WT> elements) const override
		{
			for (auto& element : elements)
			{
				ViewableList::add(index, std::move(element));
				++index;
			}
			return true;
		}

		virtual bool addAll(std::vector<WT> elements) const override
		{
			for (auto&& element : elements)
			{
				ViewableList::add(std::move(element));
			}
			return true;
		}

		virtual void clear() const override
		{
			std::vector<Event> changes;
			for (size_t i = size(); i > 0; --i)
			{
				changes.push_back(
					typename Event::Remove(static_cast<int32_t>(i - 1),
					                       &(*list[i - 1])));
			}
			for (const auto& e : changes)
			{
				change.fire(e);
			}
			list.clear();
		}

		virtual bool removeAll(std::vector<WT> elements) const override
		{
			// TO-DO faster
			//        std::unordered_set<T> set(elements.begin(), elements.end());

			bool res = false;
			for (size_t i = list.size(); i > 0; --i)
			{
				const auto& x = list[i - 1];
				if (std::count_if(elements.begin(), elements.end(),
				                  [&x](const auto& elem)
				                  {
					                  return wrapper::TransparentKeyEqual<T>()(elem, x);
				                  }) > 0)
				{
					removeAt(i - 1);
					res = true;
				}
			}
			return res;
		}

		virtual size_t size() const override
		{
			return list.size();
		}

		virtual bool empty() const override
		{
			return list.empty();
		}
	};
} // namespace rd

static_assert(std::is_move_constructible_v<rd::ViewableList<int>>,
	"Is move constructible from ViewableList<int>");

#endif	  // RD_CPP_CORE_VIEWABLELIST_H
