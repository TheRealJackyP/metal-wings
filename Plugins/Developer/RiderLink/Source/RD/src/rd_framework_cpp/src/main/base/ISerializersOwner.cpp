#include "ISerializersOwner.h"

namespace rd
{
	void ISerializersOwner::registry(const Serializers& serializers) const
	{
		auto it = used.insert(&serializers);
		if (!it.second)
		{
			return;
		}

		registerSerializersCore(serializers);
	}
} // namespace rd
