// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformDirectory.h>
#include <Platform\PlatformMemory.h>
#include <Common\StringUtility.h>
#include <Windows.h>
#include <filesystem>

using namespace std::experimental::filesystem;

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class DirectoryHandle
		{
		public:
			DirectoryHandle(cwstr Path) :
				ReferenceCount(1)
			{
				Iterator = directory_iterator(Path);
			}

			directory_iterator Iterator;
			unsigned int ReferenceCount;
		};

		PlatformDirectory::DirectoryIterator::DirectoryIterator(cwstr Path) :
			m_Handle(new DirectoryHandle(Path))
		{
		}

		PlatformDirectory::DirectoryIterator::DirectoryIterator(const DirectoryIterator &Other) :
			m_Handle(Other.m_Handle)
		{
			++reinterpret_cast<DirectoryHandle*>(m_Handle)->ReferenceCount;
		}

		PlatformDirectory::DirectoryIterator::~DirectoryIterator(void)
		{
			DropHandle();
		}

		PlatformDirectory::DirectoryIterator &PlatformDirectory::DirectoryIterator::operator=(const PlatformDirectory::DirectoryIterator &Other)
		{
			DropHandle();

			m_Handle = Other.m_Handle;
			++reinterpret_cast<DirectoryHandle*>(m_Handle)->ReferenceCount;

			return *this;
		}

		PlatformDirectory::DirectoryIterator &PlatformDirectory::DirectoryIterator::operator++(void)
		{
			directory_iterator &it = reinterpret_cast<DirectoryHandle*>(m_Handle)->Iterator;

			if (++it == directory_iterator())
				DropHandle();

			return *this;
		}

		bool PlatformDirectory::DirectoryIterator::operator==(const PlatformDirectory::DirectoryIterator &Other)
		{
			return (m_Handle == Other.m_Handle);
		}

		bool PlatformDirectory::DirectoryIterator::operator!=(const PlatformDirectory::DirectoryIterator &Other)
		{
			return (m_Handle != Other.m_Handle);
		}

		const cwstr &PlatformDirectory::DirectoryIterator::GetPath(void)
		{
			return reinterpret_cast<DirectoryHandle*>(m_Handle)->Iterator->path().c_str();
		}

		void PlatformDirectory::DirectoryIterator::DropHandle(void)
		{
			if (m_Handle == nullptr)
				return;

			if (--reinterpret_cast<DirectoryHandle*>(m_Handle)->ReferenceCount == 0)
				delete m_Handle;

			m_Handle = nullptr;
		}

		void BuildPath(cwstr Path, wstr OutPath)
		{
			uint32 len = StringUtility::GetLength(Path);

			PlatformMemory::Copy(Path, OutPath, len);

			if (OutPath[len] != StringUtility::Character<char16, '\\'>::Value && OutPath[len] != StringUtility::Character<char16, '/'>::Value)
				OutPath[len++] = StringUtility::Character<char16, '/'>::Value;

			OutPath[len++] = StringUtility::Character<char16, '*'>::Value;
			OutPath[len++] = StringUtility::Character<char16, '.'>::Value;
			OutPath[len++] = StringUtility::Character<char16, '*'>::Value;
			OutPath[len++] = StringUtility::Character<char16, '\0'>::Value;
		}

		bool PlatformDirectory::Create(cwstr Path)
		{

			return false;
		}
		void PlatformDirectory::Delete(cwstr Path)
		{

		}

		void PlatformDirectory::Delete(cwstr Path, bool Recursive)
		{

		}

		bool PlatformDirectory::Exists(cwstr Path)
		{
			return false;
		}

		void PlatformDirectory::GetDirectories(cwstr Path)
		{

		}

		void PlatformDirectory::GetDirectories(cwstr Path, cwstr SearchPattern)
		{

		}

		void PlatformDirectory::GetDirectories(cwstr Path, cwstr SearchPattern, SearchOptions SearchOption)
		{

		}

		PlatformDirectory::DirectoryIterator PlatformDirectory::GetFiles(cwstr Path)
		{
			//WIN32_FIND_DATAW fd;

			//static char16 path[MAX_PATH_LENGTH];
			//BuildPath(Path, path);

			//HANDLE handle = FindFirstFileW(path, &fd);

			//if (handle == INVALID_HANDLE_VALUE)
			//	return WindowsDirectoryIterator();

			//return WindowsDirectoryIterator(handle);

			return DirectoryIterator(Path);
		}

		unsigned int PlatformDirectory::GetFiles(cwstr Path, cwstr SearchPattern, cwstr *Files, unsigned int Count)
		{
			return 0;
		}

		unsigned int PlatformDirectory::GetFiles(cwstr Path, cwstr SearchPattern, SearchOptions SearchOption, cwstr *Files, unsigned int Count)
		{
			return 0;
		}

		void PlatformDirectory::Move(cwstr SrceDirName, cwstr DestDirName)
		{

		}
	}
}
#endif