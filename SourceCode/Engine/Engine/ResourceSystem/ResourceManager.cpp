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

			path = Path::Normalize(path);
			path.Split("/");

			path = Path::Normalize(path);
			bool val = Path::HasExtension(path);
			String val1 = Path::GetExtension(path);
			String val2 = Path::GetFileName(path);
			String val3 = Path::GetFileNameWithoutExtension(path);
			String val4 = Path::GetDirectoryName(path);

			String val5(Path::Combine(TEXT("D:\\"), TEXT("Test"), TEXT("/1.txt")));
		}

		ResourceManager::~ResourceManager(void)
		{
		}
	}
}