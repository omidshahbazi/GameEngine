// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformDirectory.h>
#include <Platform\PlatformMemory.h>
#include <Common\StringUtility.h>
#include <Windows.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class WindowsDirectoryIterator : public PlatformDirectory::DirectoryIterator
		{
		private:
			class DirectoryHandle
			{
			public:
				DirectoryHandle(HANDLE Handle) :
					Handle(Handle),
					ReferenceCount(1)
				{
				}

				HANDLE Handle;
				unsigned int ReferenceCount;
			};

		public:
			WindowsDirectoryIterator(void) :
				m_Handle(nullptr)
			{
			}

			WindowsDirectoryIterator(HANDLE Handle) :
				m_Handle(new DirectoryHandle(Handle))
			{
			}

			WindowsDirectoryIterator(const WindowsDirectoryIterator &Other) :
				m_Handle(Other.m_Handle)
			{
				++m_Handle->ReferenceCount;
			}

			~WindowsDirectoryIterator(void)
			{
				DropHandle();
			}

			WindowsDirectoryIterator &operator=(const WindowsDirectoryIterator &Other)
			{
				DropHandle();

				m_Handle = Other.m_Handle;
				++m_Handle->ReferenceCount;

				return *this;
			}

		private:
			void DropHandle(void)
			{
				if (m_Handle == nullptr)
					return;

				if (--m_Handle->ReferenceCount == 0)
				{
					FindClose(m_Handle->Handle);
					delete m_Handle;
				}
			}

		private:
			DirectoryHandle *m_Handle;
		};

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
			WIN32_FIND_DATAW fd;

			static char16 path[MAX_PATH_LENGTH];
			BuildPath(Path, path);

			HANDLE handle = FindFirstFileW(path, &fd);

			if (handle == INVALID_HANDLE_VALUE)
				return WindowsDirectoryIterator();

			return WindowsDirectoryIterator(handle);
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