// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>

namespace Engine
{
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

			ByteBuffer *Compile(const WString &Extension, ByteBuffer *Buffer);

			Resource *Create(ByteBuffer *Buffer);
		};
	}
}

#endif