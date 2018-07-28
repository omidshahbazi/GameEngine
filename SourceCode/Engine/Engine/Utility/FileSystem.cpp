// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Platform\PlatformOS.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>

namespace Engine
{
	using namespace Platform;

	namespace Utility
	{
		const WString &FileSystem::GetExecutablePath(void)
		{
			static WString path;
			static bool initialized = false;

			if (!initialized)
			{
				char16 buffer[MAX_PATH_LENGTH + 1];
				PlatformOS::GetExecutablePath(buffer);
				path = buffer;
				initialized = true;
			}

			return path;
		}

		const WString &FileSystem::GetWorkingPath(void)
		{
			static WString path;
			static bool initialized = false;

			if (!initialized)
			{
				path = GetExecutablePath();
				path = Path::GetDirectoryName(path);
			}

			return path;
		}

		const WString &FileSystem::GetWorkingPath(void)
		{
			static WString path;
			static bool initialized = false;

			if (!initialized)
			{
				path = ablePath();
				path = Path::GetDirectoryName(path);
			}

			return path;
		}

		void FileSystem::GetFiles(const WString &Path, Vector<WString> &Files, SearchOptions SearchOption)
		{
			auto fileIT = PlatformDirectory::GetFiles(Path.GetValue());

			for (; fileIT != PlatformDirectory::DirectoryIterator(); ++fileIT)
				Files.Add(fileIT.GetPath());

			if (SearchOption == SearchOptions::All)
			{
				auto dirIT = PlatformDirectory::GetDirectories(Path.GetValue());

				for (; dirIT != PlatformDirectory::DirectoryIterator(); ++dirIT)
					GetFiles(dirIT.GetPath(), Files, SearchOption);
			}
		}
	}
}