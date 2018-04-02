// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\FileSystem.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	namespace Utility
	{
		void FileSystem::GetExecutingPath(Path & Path)
		{
			char8 path[MAX_PATH_LENGTH + 1];
			PlatformFile::GetExecutingPath(path);
			Path = path;
		}
	}
}