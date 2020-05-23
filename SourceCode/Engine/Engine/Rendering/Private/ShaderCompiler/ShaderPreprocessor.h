// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PREPROCESSOR_H
#define SHADER_PREPROCESSOR_H

#include <Containers\Strings.h>
#include <Rendering\ShaderInfo.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				//TODO: Add preprocessors like #if/#else/#elif and #include
				class ShaderPreprocessor
				{
				private:
					typedef std::function<bool(String& Source)> ProcessFunction;

				public:
					typedef std::function<bool(const String&, String& Source)> IncludeCallback;

				public:
					ShaderPreprocessor(void);

					bool Preprocess(const String& InSource, const ShaderInfo::DefineList& Defines, IncludeCallback IncludeCallback, String& OutSource);

				private:
					bool Preprocess(String& OutSource);

					bool Process_include(String& OutSource);
					bool Process_define(String& OutSource);
					bool Process_ifdef(String& OutSource);
					int32 Process_ifdef(String& OutSource, bool ReverseCheck, const String& Define, uint32 StartIndex);

				private:
					ShaderInfo::DefineList m_Defines;
					IncludeCallback m_IncludeCallback;
					Vector<std::shared_ptr<ProcessFunction>> m_Processors;
				};
			}
		}
	}
}

#endif