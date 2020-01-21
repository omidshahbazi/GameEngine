// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WrapperTool\WrapperGenerator.h>
#include <WrapperTool\HeaderParser.h>
#include <Utility\Path.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	using namespace Utility;
	using namespace Platform;

	namespace WrapperTool
	{
		const String FILE_HEADER = "// Copyright 2016-2020 ?????????????. All Rights Reserved.\n// This file generated with ?????????????? based on what\n// you wrote in the original file, do not change it manually\n";

		String ReadFromFile(const WString& FilePath)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Input);

			uint64 size = PlatformFile::Size(handle);
			StringStream str(size);
			size = PlatformFile::Read(handle, str.GetBuffer(), size);

			PlatformFile::Close(handle);

			return str.GetBuffer();
		}

		void WriteToFile(const WString& FilePath, const String& Data)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);

			PlatformFile::Write(handle, Data.GetValue());

			PlatformFile::Close(handle);
		}

		bool WrapperGenerator::Generate(void)
		{
			String content = ReadFromFile(m_FilePath.ChangeType<char16>());
			HeaderParser hp(content);

			String str = "";

			str += "#include \"" + m_FilePath + "\"\n"
				"using namespace Engine::CoreSystem;\n"
				"void* Core_GetInstance(void)"
				"{"
				"	return Core::GetInstance();"
				"}"
				""
				"void Core_Initialize(void* Instance)"
				"{"
				"	reinterpret_cast<Core*>(Instance)->Initialize();"
				"}";

			WriteToFile((m_OutputBaseFileName + ".h").ChangeType<char16>(), str);
			WriteToFile((m_OutputBaseFileName + ".cpp").ChangeType<char16>(), "#include \"" + m_OutputBaseFileName + ".h\"");

			//TypesList types;
			//hp.Parse(types);

			//if (types.GetSize() == 0)
			//	return false;

			return true;
		}
	}
}
