// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\ReflectionGenerator.h>
#include <ReflectionGenerator\Private\HeaderParser.h>
#include <ReflectionGenerator\Private\HeaderFileGenerator.h>
#include <ReflectionGenerator\Private\CompileFileGenerator.h>
#include <FileUtility\Path.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace Platform;

	namespace ReflectionGenerator
	{
		using namespace Private;

		String ReadFromFile(const WString& FilePath)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Input);

			uint64 size = PlatformFile::Size(handle);
			StringStream str(size);
			size = PlatformFile::Read(handle, str.GetBuffer(), size);

			PlatformFile::Close(handle);

			return str.GetBuffer();
		}

		bool ReflectionGenerator::Generate(AllocatorBase* Allocator, const WString& FilePath, const WString& OutputBaseFileName)
		{
			String content = ReadFromFile(FilePath.ChangeType<char16>());
			HeaderParser hp(Allocator, content);

			TypeList types;
			hp.Parse(types);

			if (types.GetSize() == 0)
				return false;

			HeaderFileGenerator::Generate(types, OutputBaseFileName + L".h");
			CompileFileGenerator::Generate(types, OutputBaseFileName + L".cpp");

			for (auto& type : types)
			{
				Destruct(type);
				DeallocateMemory(Allocator, type);
			}

			return true;
		}
	}
}
