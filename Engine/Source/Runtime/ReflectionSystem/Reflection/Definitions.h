// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define REFLECTION_OBJECT(...)
#define REFLECTION_ENUM(...)
#define REFLECTION_PROPERTY(...)
#define REFLECTION_FUNCTION(...)

#define REFLECTION_ABSTRACT
#define REFLECTION_READONLY

#define TypeOf(Type) Type::GetType()

namespace Engine
{
	namespace Reflection
	{
		enum class AccessSpecifiers
		{
			None = 0,
			Private = 2,
			Protected = 4,
			Public = 8
		};
	}
}

#endif