// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef VALUE_TYPES_H
#define VALUE_TYPES_H
namespace Engine
{
	namespace Common
	{
		enum class ValueTypes
		{
			None = 0,

			VoidPointer,

			Bool,

			Int8,
			Int16,
			Int32,
			Int64,

			UInt8,
			UInt16,
			UInt32,
			UInt64,

			Float32,
			Float64,

			String,
			WString,

			Vector2F,
			Vector3F,
			Vector4F,
			Matrix4F
		};
	}
}
#endif
