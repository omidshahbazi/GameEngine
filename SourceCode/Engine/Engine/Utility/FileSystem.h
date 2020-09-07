// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <Containers\Strings.h>
#include <Containers\List.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		class UTILITY_API FileSystem
		{
		private:
			struct Fields
			{
			public:
				WString ExecutablePath;
				WString ExecutableDirectory;
				WString WorkingPath;
			};

		public:
			enum class SearchOptions
			{
				TopOnly = 0,
				All
			};

		public:
			static void Initialize(void);
			static void Deinitialize(void);

			static const WString& GetExecutablePath(void);

			static const WString& GetExecutableDirectory(void);

			static void SetWorkingPath(const WString& Value);
			static const WString& GetWorkingPath(void);

			static void Delete(const WString& Path);

			static bool Exists(const WString& Path);

			static void GetFiles(const String& Path, WStringList& Files, SearchOptions SearchOption = SearchOptions::TopOnly)
			{
				GetFiles(Path.ChangeType<char16>(), Files, SearchOption);
			}
			static void GetFiles(const WString& Path, WStringList& Files, SearchOptions SearchOption = SearchOptions::TopOnly);

			static void GetFiles(const String& Path, WStringList& Files, const String& Pattern, SearchOptions SearchOption = SearchOptions::TopOnly)
			{
				GetFiles(Path.ChangeType<char16>(), Files, Pattern.ChangeType<char16>(), SearchOption);
			}
			static void GetFiles(const WString& Path, WStringList& Files, const WString& Pattern, SearchOptions SearchOption = SearchOptions::TopOnly);

			static uint64 GetSize(const String& Path)
			{
				return GetSize(Path.ChangeType<char16>());
			}
			static uint64 GetSize(const WString& Path);

			static bool ReadAllBytes(const String& Path, byte* Data, uint64 Count)
			{
				return ReadAllBytes(Path.ChangeType<char16>(), Data, Count);
			}
			static bool ReadAllBytes(const WString& Path, byte* Data, uint64 Count);

			static bool WriteAllBytes(const String& Path, const byte* Data, uint64 Count)
			{
				return WriteAllBytes(Path.ChangeType<char16>(), Data, Count);
			}
			static bool WriteAllBytes(const WString& Path, const byte* Data, uint64 Count);

			static bool ReadAllText(const String& Path, String* Data)
			{
				return ReadAllText(Path.ChangeType<char16>(), Data);
			}
			static bool ReadAllText(const WString& Path, String* Data);

			static bool ReadAllText(const String& Path, WString* Data)
			{
				return ReadAllText(Path.ChangeType<char16>(), Data);
			}
			static bool ReadAllText(const WString& Path, WString* Data);

			static bool WriteAllText(const String& Path, const String& Data)
			{
				return WriteAllText(Path.ChangeType<char16>(), Data.ChangeType<char16>());
			}
			static bool WriteAllText(const WString& Path, const String& Data)
			{
				return WriteAllText(Path, Data.ChangeType<char16>());
			}
			static bool WriteAllText(const String& Path, const WString& Data)
			{
				return WriteAllText(Path.ChangeType<char16>(), Data);
			}
			static bool WriteAllText(const WString& Path, const WString& Data);


		private:
			static bool m_Initialized;
			static Fields* m_Fields;
		};
	}
}

#endif