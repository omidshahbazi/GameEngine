// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include <Rendering\RenderingCommon.h>
#include <Rendering\ProgramInfo.h>
#include <Rendering\CompiledProgramInfo.h>
#include <Rendering\ProgramDataTypes.h>
#include <Rendering\Private\ProgramCompiler\CompileOutputInfo.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Strings.h>
#include <Containers\ListenerContainer.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				class RENDERING_API Compiler
				{
				public:
					static cstr ENTRY_POINT_NAME;

					typedef std::function<void(const String& Message, uint16 Line)> ErrorFunction;

				public:
					class RENDERING_API IListener
					{
					public:
						virtual bool FetchShaderSource(const String& Name, String& Source)
						{
							return false;
						}
					};

					SINGLETON_DECLARATION(Compiler);

					LISTENER_DECLARATION(IListener);

				private:
					Compiler(void);

				public:
					bool Compile(const ProgramInfo* Info, DeviceTypes DeviceType, CompileOutputInfo& Output, ErrorFunction OnError = nullptr);
					bool Compile(const ProgramInfo* Info, const DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, CompileOutputInfo* Outputs, ErrorFunction OnError = nullptr);
				};
			}
		}
	}
}

#endif