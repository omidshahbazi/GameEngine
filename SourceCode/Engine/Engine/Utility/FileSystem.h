// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		class UTILITY_API FileSystem
		{
		public:
			enum class SearchOptions
			{
				TopOnly = 0,
				All
			};

		public:
			static const WString &GetExecutablePath(void);

			static void SetWorkingPath(const WString &Value);
			static const WString &GetWorkingPath(void);

			static void GetFiles(const WString &Path, Vector<WString> &Files, SearchOptions SearchOption = SearchOptions::TopOnly);

			static uint64 GetSize(const WString &Path);
			static void ReadAllBytes(const WString &Path, byte *Data, uint64 Count);
			static void WriteAllBytes(const WString &Path, const byte *Data, uint64 Size);

		private:
			static void Initialize(void);

		private:
			static bool m_Initialized;
			static WString m_ExecutablePath;
			static WString m_WorkingPath;
		};
	}
}

#endif