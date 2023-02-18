#ifndef RD_CPP_FRAMEWORK_IRDBINDABLE_H
#define RD_CPP_FRAMEWORK_IRDBINDABLE_H

#include "IRdDynamic.h"
#include "lifetime/Lifetime.h"
#include "protocol/RdId.h"

#include <rd_framework_export.h>

namespace rd
{
	class Identities;
}

namespace rd
{
	/**
	 * \brief A non-root node in an object graph which can be synchronized with its remote copy over a network or
	 * a similar connection.
	 */
	class RD_FRAMEWORK_API IRdBindable : public IRdDynamic
	{
	public:
		// region ctor/dtor

		IRdBindable() = default;

		IRdBindable(IRdBindable&& other) = default;

		IRdBindable& operator=(IRdBindable&& other) = default;

		virtual ~IRdBindable() override = default;
		// endregion

		virtual void set_id(RdId id) const = 0;
		virtual RdId get_id() const = 0;

		/**
		 * \brief Inserts the node into the object graph under the given [parent] and assigns the specified [name] to it.
		 * The node will be removed from the graph when the specified [lf] lifetime is terminated.
		 *
		 * \param lf lifetime of node.
		 * \param parent to whom bind.
		 * \param name specified name of node.
		 */
		virtual void bind(Lifetime lf, const IRdDynamic* parent,
		                  string_view name) const = 0;

		/**
		 * \brief Assigns IDs to this node and its child nodes in the graph.
		 *
		 * \param identities to generate unique identifiers for children.
		 * \param id which is assigned to this node.
		 */
		virtual void identify(const Identities& identities, const RdId& id) const = 0;
	};

	template <typename T>
	std::enable_if_t<!util::is_base_of_v<IRdBindable, std::decay_t<T>>>
	identifyPolymorphic(
		T&&, const Identities& /*identities*/, const RdId& /*id*/)
	{
	}

	// template <>
	inline void identifyPolymorphic(const IRdBindable& that,
	                                const Identities& identities, const RdId& id)
	{
		that.identify(identities, id);
	}

	template <typename T>
	std::enable_if_t<util::is_base_of_v<IRdBindable, T>> identifyPolymorphic(
		const std::vector<T>& that, const Identities& identities, const RdId& id)
	{
		for (size_t i = 0; i < that.size(); ++i)
		{
			that[i].identify(identities, id.mix(static_cast<int32_t>(i)));
		}
	}

	template <typename T>
	std::enable_if_t<!util::is_base_of_v<IRdBindable, std::decay_t<T>>> bindPolymorphic(
		T&&, Lifetime /*lf*/, const IRdDynamic* /*parent*/, string_view /*name*/)
	{
	}

	inline void bindPolymorphic(const IRdBindable& that, Lifetime lf,
	                            const IRdDynamic* parent, string_view name)
	{
		that.bind(lf, parent, name);
	}

	template <typename T>
	std::enable_if_t<util::is_base_of_v<IRdBindable, T>> bindPolymorphic(
		const std::vector<T>& that, Lifetime lf, const IRdDynamic* parent,
		string_view name)
	{
		for (auto& obj : that)
		{
			obj.bind(lf, parent, name);
		}
	}
} // namespace rd

#endif	  // RD_CPP_FRAMEWORK_IRDBINDABLE_H
