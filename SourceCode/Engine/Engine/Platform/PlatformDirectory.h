// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_DIRECTORY_H
#define PLATFORM_DIRECTORY_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
#define MAX_PATH_LENGTH 512

		class PLATFORM_API PlatformDirectory
		{
		public:
			class PLATFORM_API DirectoryIterator
			{
			public:
				DirectoryIterator(void) :
					m_Handle(nullptr)
				{
				}

				DirectoryIterator(bool FileSearch, cwstr Path, cwstr SearchPattern = nullptr);

				DirectoryIterator(const DirectoryIterator &Other);

				~DirectoryIterator(void);

				DirectoryIterator &operator=(const DirectoryIterator &Other);

				DirectoryIterator &operator++(void);

				bool operator==(const DirectoryIterator &Other);
				bool operator!=(const DirectoryIterator &Other);

				cwstr GetPath(void);

			private:
				void IncrementToMeetConditions(bool IsFirstOne);

			private:
				void DropHandle(void);

			private:
				void *m_Handle;
			};

		public:
			static bool Create(cwstr Path);

			static bool Delete(cwstr Path);

			static bool Exists(cwstr Path);

			static DirectoryIterator GetDirectories(cwstr Path);

			static DirectoryIterator GetFiles(cwstr Path, cwstr SearchPattern = nullptr);

			static void Move(cwstr SrceDirName, cwstr DestDirName);

			static void SetWokringDirectory(cwstr Path);
		};
	}
}

#endif