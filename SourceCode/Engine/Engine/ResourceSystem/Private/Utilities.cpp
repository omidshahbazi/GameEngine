// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\Utilities.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Common\BitwiseUtils.h>
#include <Platform\PlatformFile.h>
#include <DataUtility\Hash.h>

namespace Engine
{
	using namespace DataUtility;

	namespace ResourceSystem
	{
		namespace Private
		{
			cwstr Utilities::DATA_EXTENSION = L".data";

			uint32 Utilities::GetHash(const GUID& GUID)
			{
				return Hash::CRC32(&GUID, 1);
			}

			bool Utilities::ReadDataFile(ByteBuffer& Buffer, const WString& Path)
			{
				auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input | PlatformFile::OpenModes::Binary);

				if (handle == 0)
					return false;

				uint64 fileSize = PlatformFile::Size(handle);

				Buffer.Extend(fileSize);

				if ((fileSize = PlatformFile::Read(handle, Buffer.GetBuffer(), fileSize)) == 0)
					return false;

				PlatformFile::Close(handle);

				return true;
			}

			bool Utilities::WriteDataFile(const WString& Path, const ByteBuffer& Buffer)
			{
				auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output | PlatformFile::OpenModes::Binary);

				if (handle == 0)
					return false;

				PlatformFile::Write(handle, Buffer.GetBuffer(), Buffer.GetSize());

				PlatformFile::Close(handle);

				return true;
			}

			WString Utilities::GetDataFileName(const GUID& GUID)
			{
				WStringStream stream(ResourceSystemAllocators::ResourceAllocator);
				uint32 hash = GetHash(GUID);
				stream << hash << DATA_EXTENSION << '\0';
				return stream.GetBuffer();
			}
		}
	}
}