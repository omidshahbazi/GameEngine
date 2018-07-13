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

			public:
				DirectoryIterator(void) :
					m_Handle(nullptr)
				{
				}

				DirectoryIterator(cwstr Path);

				DirectoryIterator(const DirectoryIterator &Other);

				~DirectoryIterator(void);

				DirectoryIterator &operator=(const DirectoryIterator &Other);

				DirectoryIterator &operator++(void);

				bool operator==(const DirectoryIterator &Other);
				bool operator!=(const DirectoryIterator &Other);

				const cwstr &GetPath(void);

			private:
				void DropHandle(void);

			private:
				void *m_Handle;
			};

			enum class SearchOptions
			{
				TopOnly = 0,
				All = 1
			};

		public:
			static bool Create(cwstr Path);

			static void Delete(cwstr Path);
			static void Delete(cwstr Path, bool Recursive);

			static bool Exists(cwstr Path);

			static void GetDirectories(cwstr Path);
			static void GetDirectories(cwstr Path, cwstr SearchPattern);
			static void GetDirectories(cwstr Path, cwstr SearchPattern, SearchOptions SearchOption);

			static DirectoryIterator GetFiles(cwstr Path);
			static unsigned int GetFiles(cwstr Path, cwstr SearchPattern, cwstr *Files, unsigned int Count);
			static unsigned int GetFiles(cwstr Path, cwstr SearchPattern, SearchOptions SearchOption, cwstr *Files, unsigned int Count);

			static void Move(cwstr SrceDirName, cwstr DestDirName);
		};
	}
}

#endif