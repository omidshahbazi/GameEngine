// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace ResourceSystem
	{
		class RESOURCESYSTEM_API ResourceManager
		{
			SINGLETON_DEFINITION(ResourceManager)

		public:
			ResourceManager(void);
			~ResourceManager(void);

			void Compile(void);
		};
	}
}

#endif