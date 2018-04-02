// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <Utility\FileSystem.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility;

	namespace ResourceSystem
	{
		SINGLETON_DECLARATION(ResourceManager)

		ResourceManager::ResourceManager(void)
		{
			Path path;
			FileSystem::GetExecutingPath(path);


		}

		ResourceManager::~ResourceManager(void)
		{
		}
	}
}