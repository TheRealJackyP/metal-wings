#include "serialization/ISerializable.h"

#include "std/hash.h"

namespace rd
{
	size_t IPolymorphicSerializable::hashCode() const noexcept
	{
		return rd::hash<const void*>()(this);
	}

	bool operator==(const IPolymorphicSerializable& lhs,
	                const IPolymorphicSerializable& rhs)
	{
		return lhs.equals(rhs);
	}

	bool operator!=(const IPolymorphicSerializable& lhs,
	                const IPolymorphicSerializable& rhs)
	{
		return !(lhs == rhs);
	}
} // namespace rd
