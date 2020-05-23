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
				const String IFDEF_KEYWORD = "#ifdef";
				const String IFNDEF_KEYWORD = "#ifndef";
				const String ELSE_KEYWORD = "#else";
				const String ENDIF_KEYWORD = "#endif";

				bool IsCommented(const String& Source, uint32 Index)
				{
					for (int32 i = Index - 1; i >= 0; --i)
					{
						if (Source[i] == '\n' || Source[i] == '\r')
							return false;

						if (Source[i] == '/' && Source[i + 1] == '/')
							return true;
					}

					return false;
				}

				int32 FirstIndexOf(const String& Source, const String& Value, uint32 StartIndex)
				{
					int32 index = StartIndex;

					while (true)
					{
						index = Source.FirstIndexOf(Value, index);

						if (index == -1)
							return -1;

						if (IsCommented(Source, index))
						{
							++index;
							continue;
						}

						return index;
					}

					return -1;
				}

				int32 FirstIndexOfAny(const String& Source, const String& Any, uint32 StartIndex)
				{
					for (uint32 i = 0; i < Any.GetLength(); ++i)
					{
						int32 index = FirstIndexOf(Source, Any[i], StartIndex);

						if (index == -1)
							continue;

						return index;
					}

					return -1;
				}

				bool IsDefined(const ShaderInfo::DefineList& Defines, const String& Define)
				{
					for each (auto & define in Defines)
					{
						if (define.Name == Define)
							return true;
					}

					return false;
				}

				ShaderPreprocessor::ShaderPreprocessor(void)
				{
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_include(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_define(OutSource); }));
					m_Processors.Add(std::make_shared<ProcessFunction>([&](String& OutSource) { return Process_ifdef(OutSource); }));
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
						index = FirstIndexOf(OutSource, KEYWORD, index);
						if (index == -1)
							break;

						int32 fromIndex = FirstIndexOf(OutSource, '<', index + KEYWORD.GetLength());
						if (fromIndex == -1)
							return false;
						++fromIndex;

						int32 toIndex = FirstIndexOf(OutSource, '>', fromIndex + 1);
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
						index = FirstIndexOf(OutSource, KEYWORD, index);
						if (index == -1)
							break;

						int32 toIndex = FirstIndexOfAny(OutSource, "\n\r", index + KEYWORD.GetLength());
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

						OutSource = OutSource.Remove(index, toIndex - index);
					}

					return true;
				}

				bool ShaderPreprocessor::Process_ifdef(String& OutSource)
				{
					int32 index = 0;
					while (true)
					{
						int32 result = Process_ifdef(OutSource, false, "", 0);

						if (result == 1)
							continue;

						if (result == 0)
							return false;

						if (result == -1)
							break;
					}

					return true;
				}

				int32 ShaderPreprocessor::Process_ifdef(String& OutSource, bool ReverseCheck, const String& Define, uint32 StartIndex)
				{
					int32 endBlockIndex = -1;
					bool hasElseBlock = false;

					if ((endBlockIndex = FirstIndexOf(OutSource, ELSE_KEYWORD, StartIndex)) != -1)
						hasElseBlock = true;
					else if ((endBlockIndex = FirstIndexOf(OutSource, ENDIF_KEYWORD, StartIndex)) == -1)
						return 0;

					int32 index = -1;
					if (((index = FirstIndexOf(OutSource, IFDEF_KEYWORD, StartIndex)) != -1 || (index = FirstIndexOf(OutSource, IFNDEF_KEYWORD, StartIndex)) != -1) &&
						index < endBlockIndex)
					{
						bool reverseCheck = (FirstIndexOf(OutSource, IFNDEF_KEYWORD, index) == index);

						int32 endLineIndex = FirstIndexOfAny(OutSource, "\n\r", index);
						if (endLineIndex == -1)
							return 0;

						StringList parts = OutSource.SubString(index, endLineIndex - index).Split(' ');
						if (parts.GetSize() != 2)
							return 0;

						if (Process_ifdef(OutSource, reverseCheck, parts[1], index + 1) == 0)
							return 0;
					}

					if (Define.GetLength() == 0)
						return 1;

					bool shouldRemoveBlock = (IsDefined(m_Defines, Define) == ReverseCheck);

					--StartIndex;

					int32 endLineIndex = FirstIndexOfAny(OutSource, "\n\r", StartIndex);
					if (endLineIndex == -1)
						return 0;

					OutSource = OutSource.Remove(StartIndex, endLineIndex - StartIndex);

					if ((endBlockIndex = FirstIndexOf(OutSource, ELSE_KEYWORD, StartIndex)) == -1 && (endBlockIndex = FirstIndexOf(OutSource, ENDIF_KEYWORD, StartIndex)) == -1)
						return 0;

					if (shouldRemoveBlock)
						OutSource = OutSource.Remove(StartIndex, endBlockIndex - StartIndex);

					int32 endIfIndex = -1;
					if (hasElseBlock)
					{
						int32 elseIndex = -1;
						if ((elseIndex = FirstIndexOf(OutSource, ELSE_KEYWORD, StartIndex)) == -1)
							return 0;

						endLineIndex = FirstIndexOfAny(OutSource, "\n\r", elseIndex);
						if (endLineIndex == -1)
							return 0;

						OutSource = OutSource.Remove(elseIndex, endLineIndex - elseIndex);

						if ((endIfIndex = FirstIndexOf(OutSource, ENDIF_KEYWORD, elseIndex)) == -1)
							return 0;

						if (!shouldRemoveBlock)
							OutSource = OutSource.Remove(endBlockIndex, endIfIndex - endBlockIndex);
					}

					if ((endIfIndex = FirstIndexOf(OutSource, ENDIF_KEYWORD, endBlockIndex)) == -1)
						return 0;

					endLineIndex = FirstIndexOfAny(OutSource, "\n\r", endBlockIndex);
					if (endLineIndex == -1)
						return 0;

					OutSource = OutSource.Remove(endIfIndex, endLineIndex - endIfIndex);

					return 1;
				}
			}
		}
	}
}