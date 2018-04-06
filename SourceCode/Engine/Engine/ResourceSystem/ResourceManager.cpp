// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility;

	namespace ResourceSystem
	{
		SINGLETON_DECLARATION(ResourceManager)

		ResourceManager::ResourceManager(void)
		{
			String path = FileSystem::GetExecutingPath();

			bool val = Path::HasExtension(path);
			String val1 = Path::GetExtension(path);
			String val2 = Path::GetFileNameWithoutExtension(path);

			//WString str;
			//Path::Normalize(str);
		}

		ResourceManager::~ResourceManager(void)
		{
		}
	}
}