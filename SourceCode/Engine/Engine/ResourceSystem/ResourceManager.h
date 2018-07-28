// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		class Resource;

		class RESOURCESYSTEM_API ResourceManager
		{
			SINGLETON_DEFINITION(ResourceManager)

		public:
			ResourceManager(void);
			~ResourceManager(void);

			Resource *Load(const WString &Path);
			Resource *Load(const String &Path);

		private:
			void Compile(void);
		};
	}
}

#endif