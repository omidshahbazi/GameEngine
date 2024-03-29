// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Platform\PlatformDirectory.h>
#include <experimental\filesystem>

using namespace std::experimental::filesystem;

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class DirectoryHandle
		{
		public:
			DirectoryHandle(bool FileSearch, cwstr Path, cwstr SearchPattern) :
				FileSearch(FileSearch),
				ReferenceCount(1),
				SearchPattern(SearchPattern)
			{
				Iterator = directory_iterator(Path);
			}

			bool FileSearch;
			directory_iterator Iterator;
			uint32 ReferenceCount;
			cwstr SearchPattern;
		};

		PlatformDirectory::DirectoryIterator::DirectoryIterator(bool FileSearch, cwstr Path, cwstr SearchPattern) :
			m_Handle(new DirectoryHandle(FileSearch, Path, SearchPattern))
		{
			IncrementToMeetConditions(true);
		}

		PlatformDirectory::DirectoryIterator::DirectoryIterator(const DirectoryIterator& Other) :
			m_Handle(Other.m_Handle)
		{
			++ReinterpretCast(DirectoryHandle*, m_Handle)->ReferenceCount;
		}

		PlatformDirectory::DirectoryIterator::~DirectoryIterator(void)
		{
			DropHandle();
		}

		PlatformDirectory::DirectoryIterator& PlatformDirectory::DirectoryIterator::operator=(const PlatformDirectory::DirectoryIterator& Other)
		{
			DropHandle();

			m_Handle = Other.m_Handle;
			++ReinterpretCast(DirectoryHandle*, m_Handle)->ReferenceCount;

			return *this;
		}

		PlatformDirectory::DirectoryIterator& PlatformDirectory::DirectoryIterator::operator++(void)
		{
			IncrementToMeetConditions(false);

			return *this;
		}

		bool PlatformDirectory::DirectoryIterator::operator==(const PlatformDirectory::DirectoryIterator& Other)
		{
			return (m_Handle == Other.m_Handle);
		}

		bool PlatformDirectory::DirectoryIterator::operator!=(const PlatformDirectory::DirectoryIterator& Other)
		{
			return (m_Handle != Other.m_Handle);
		}

		cwstr PlatformDirectory::DirectoryIterator::GetPath(void)
		{
			return ReinterpretCast(DirectoryHandle*, m_Handle)->Iterator->path().c_str();
		}

		void PlatformDirectory::DirectoryIterator::IncrementToMeetConditions(bool IsFirstOne)
		{
			DirectoryHandle* handle = ReinterpretCast(DirectoryHandle*, m_Handle);

			while (true)
			{
				if (IsFirstOne)
					IsFirstOne = false;
				else
					++handle->Iterator;

				if (handle->Iterator == directory_iterator())
				{
					DropHandle();
					break;
				}

				path p = handle->Iterator->path();

				if (handle->FileSearch != is_regular_file(p))
					continue;

				if (handle->SearchPattern == nullptr ? false : p.extension() != handle->SearchPattern)
					continue;

				break;
			}
		}

		void PlatformDirectory::DirectoryIterator::DropHandle(void)
		{
			if (m_Handle == nullptr)
				return;

			if (--ReinterpretCast(DirectoryHandle*, m_Handle)->ReferenceCount == 0)
				delete m_Handle;

			m_Handle = nullptr;
		}

		bool PlatformDirectory::Create(cwstr Path)
		{
			return create_directories(Path);
		}

		bool PlatformDirectory::Delete(cwstr Path)
		{
			return remove(Path);
		}

		bool PlatformDirectory::Exists(cwstr Path)
		{
			return exists(Path);
		}

		PlatformDirectory::DirectoryIterator PlatformDirectory::GetDirectories(cwstr Path)
		{
			return DirectoryIterator(false, Path);
		}

		PlatformDirectory::DirectoryIterator PlatformDirectory::GetFiles(cwstr Path, cwstr SearchPattern)
		{
			return DirectoryIterator(true, Path, SearchPattern);
		}

		void PlatformDirectory::Move(cwstr SrceDirName, cwstr DestDirName)
		{
			rename(SrceDirName, DestDirName);
		}

		void PlatformDirectory::GetWokringDirectory(cwstr* Path)
		{
			*Path = current_path().c_str();
		}

		void PlatformDirectory::SetWokringDirectory(cwstr Path)
		{
			//path p = Path;

			//if (p.is_relative())
			//{
			//	p = current_path();
			//	p.concat('/');
			//	p.concat(Path);
			//}

			current_path(Path);
		}
	}
}