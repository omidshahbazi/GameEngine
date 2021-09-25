// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef ENTITY_COMPONENT_SYSTEM_EXCEPTION_H
#define ENTITY_COMPONENT_SYSTEM_EXCEPTION_H

#include <Containers\Exception.h>

namespace Engine
{
	using namespace Containers;

	namespace EntityComponentSystem
	{
		class EntityComponentSystemException : public Exception
		{
		public:
			EntityComponentSystemException(const String& What, const String& File, uint32 Line, const String& Function) :
				Exception(Categories::EntityComponentSystem, What, File, Line, Function)
			{
			}
		};

#define THROW_IF_ENTITY_COMPONENT_SYSTEM_EXCEPTION(Condition, What) \
		if (Condition) \
			throw EntityComponentSystemException(What, DEBUG_ARGUMENTS)
	}
}
#endif
