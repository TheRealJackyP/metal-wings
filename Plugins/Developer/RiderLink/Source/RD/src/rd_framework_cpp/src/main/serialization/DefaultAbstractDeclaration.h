#ifndef RD_CPP_DEFAULTABSTRACTDECLARATION_H
#define RD_CPP_DEFAULTABSTRACTDECLARATION_H

#include "types/wrapper.h"
#include "protocol/RdId.h"
#include "serialization/ISerializable.h"
#include "base/IUnknownInstance.h"

namespace rd
{
	// region predeclared

	class SerializationCtx;

	class Buffer;
	// endregion

	class DefaultAbstractDeclaration : public IPolymorphicSerializable,
	                                   public IUnknownInstance
	{
		const static std::string not_registered_error_message;

	public:
		static Wrapper<DefaultAbstractDeclaration> readUnknownInstance(
			SerializationCtx& ctx, Buffer& buffer, const RdId& unknownId, int32_t size);

		virtual std::string type_name() const override;

		virtual bool equals(const ISerializable& serializable) const override;

		virtual void write(SerializationCtx& ctx, Buffer& buffer) const override;
	};
} // namespace rd

#endif	  // RD_CPP_DEFAULTABSTRACTDECLARATION_H
