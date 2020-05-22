// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderPreprocessor.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				ShaderPreprocessor::ShaderPreprocessor(void)
				{
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return ProcessInclude(OutSource); }));
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

				bool ShaderPreprocessor::ProcessInclude(String& OutSource)
				{
					const String INCLUDE_TEXT = "#include";

					int32 index = 0;
					while (true)
					{
						index = OutSource.FirstIndexOf(INCLUDE_TEXT, index);
						if (index == -1)
							break;

						int32 fromIndex = OutSource.FirstIndexOf('<', index + INCLUDE_TEXT.GetLength());
						if (fromIndex == -1)
							return false;
						++fromIndex;

						int32 toIndex = OutSource.FirstIndexOf('>', fromIndex + 1);
						if (toIndex == -1)
							return false;

						String includeSource;
						if (!m_IncludeCallback(OutSource.SubString(fromIndex, toIndex - fromIndex), includeSource))
							return false;

						++toIndex;

						OutSource = OutSource.Replace(OutSource.SubString(index, toIndex - index), includeSource);
					}

					return true;
				}
			}
		}
	}
}