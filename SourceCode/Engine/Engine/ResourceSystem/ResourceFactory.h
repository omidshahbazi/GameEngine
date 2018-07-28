// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace Containers
	{
		class Buffer;
	}

	using namespace Containers;

	namespace ResourceSystem
	{
		class Resource;

		class RESOURCESYSTEM_API ResourceFactory
		{
			SINGLETON_DEFINITION(ResourceFactory)

		public:
			ResourceFactory(void);
			~ResourceFactory(void);

			Resource *Create(Buffer *Buffer);
		};
	}
}

#endif