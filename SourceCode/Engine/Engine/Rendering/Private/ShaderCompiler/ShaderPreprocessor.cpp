// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderPreprocessor.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				int32 FirstIndexOfAny(const String& Value, const String& Any, uint32 StartIndex)
				{
					for (uint32 i = 0; i < Any.GetLength(); ++i)
					{
						int32 index = Value.FirstIndexOf(Any[i], StartIndex);

						if (index == -1)
							continue;

						return index;
					}

					return -1;
				}

				ShaderPreprocessor::ShaderPreprocessor(void)
				{
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_include(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_define(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_undef(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_ifdef(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_ifndef(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_else(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_endif(OutSource); }));
				}

				bool ShaderPreprocessor::Preprocess(const String& InSource, const ShaderInfo::DefineList& Defines, IncludeCallback IncludeCallback, String& OutSource)
				{
					m_Defines = Defines;
					m_IncludeCallback = IncludeCallback;

					OutSource = InSource;

					return Preprocess(OutSource);
				}

				bool ShaderPreprocessor::Preprocess(String& OutSource)
				{
					for each (auto & processor in m_Processors)
						if (!(*processor)(OutSource))
							return false;

					return true;
				}

				bool ShaderPreprocessor::Process_include(String& OutSource)
				{
					const String KEYWORD = "#include";

					int32 index = 0;
					while (true)
					{
						index = OutSource.FirstIndexOf(KEYWORD, index);
						if (index == -1)
							break;

						int32 fromIndex = OutSource.FirstIndexOf('<', index + KEYWORD.GetLength());
						if (fromIndex == -1)
							return false;
						++fromIndex;

						int32 toIndex = OutSource.FirstIndexOf('>', fromIndex + 1);
						if (toIndex == -1)
							return false;

						String includeSource;
						if (!m_IncludeCallback(OutSource.SubString(fromIndex, toIndex - fromIndex), includeSource))
							return false;

						includeSource += '\n';

						++toIndex;

						OutSource = OutSource.Replace(OutSource.SubString(index, toIndex - index), includeSource);
					}

					return true;
				}

				bool ShaderPreprocessor::Process_define(String& OutSource)
				{
					const String KEYWORD = "#define";

					int32 index = 0;
					while (true)
					{
						index = OutSource.FirstIndexOf(KEYWORD, index);
						if (index == -1)
							break;

						int32 toIndex = FirstIndexOfAny(OutSource, "\r\n", index + KEYWORD.GetLength());
						if (toIndex == -1)
							return false;

						int32 fromIndex = index + KEYWORD.GetLength();

						String define = OutSource.SubString(fromIndex, toIndex - fromIndex).TrimLeft();

						StringList parts = define.Split(' ');
						if (parts.GetSize() > 1)
							return false;

						ShaderInfo::DefineInfo info;
						info.Name = define;

						m_Defines.Add(info);

						OutSource = OutSource.Replace(OutSource.SubString(index, toIndex - index), "");
					}

					return true;
				}

				bool ShaderPreprocessor::Process_undef(String& OutSource)
				{
					const String KEYWORD = "#undef";

					return true;
				}

				bool ShaderPreprocessor::Process_ifdef(String& OutSource)
				{
					const String KEYWORD = "#ifdef";

					return true;
				}

				bool ShaderPreprocessor::Process_ifndef(String& OutSource)
				{
					const String KEYWORD = "#ifndef";

					return true;
				}

				bool ShaderPreprocessor::Process_else(String& OutSource)
				{
					const String KEYWORD = "#else";

					return true;
				}

				bool ShaderPreprocessor::Process_endif(String& OutSource)
				{
					const String KEYWORD = "#endif";

					return true;
				}
			}
		}
	}
}