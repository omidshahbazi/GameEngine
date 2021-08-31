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
#include <Containers\Delegate.h>

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

				public:
					typedef Delegate<const String&, String&> FetchShaderSourceEventHandler;

					SINGLETON_DECLARATION(Compiler);

				private:
					Compiler(void);

				public:
					void Compile(const ProgramInfo* Info, DeviceTypes DeviceType, CompileOutputInfo& Output);
					void Compile(const ProgramInfo* Info, const DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, CompileOutputInfo* Outputs);

				public:
					FetchShaderSourceEventHandler OnFetchShaderSourceEvent;
				};
			}
		}
	}
}

#endif