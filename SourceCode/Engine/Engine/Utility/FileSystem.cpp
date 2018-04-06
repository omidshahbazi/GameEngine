// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\FileSystem.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	namespace Utility
	{
		String FileSystem::GetExecutingPath(void)
		{
			char8 path[MAX_PATH_LENGTH + 1];
			PlatformFile::GetExecutingPath(path);
			return path;
		}
	}
}