// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_TO_API_COMPILER_H
#define PROGRAM_TO_API_COMPILER_H

#include <RenderCommon\RenderCommon.h>
#include <RenderDevice\ProgramInfo.h>
#include <RenderDevice\CompiledProgramInfo.h>
#include <RenderCommon\Enums.h>
#include <Containers\Strings.h>
#include <Containers\Delegate.h>
#include <MemoryManagement\Singleton.h>
#include <ASTCompiler\ASTCompilerBase.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderDevice;
	using namespace ASTCompiler;

	namespace ProgramCompiler
	{
		class PROGRAMCOMPILER_API ProgramToAPICompiler
		{
		public:
			typedef Delegate<const String&, bool&, String&> FetchShaderSourceEventHandler;

			SINGLETON_DECLARATION(ProgramToAPICompiler);

		private:
			ProgramToAPICompiler(void);
			~ProgramToAPICompiler(void);

		public:
			void Compile(const ProgramInfo* Info, DeviceTypes Type, OutputInfo& Output);
			void Compile(const ProgramInfo* Info, const DeviceTypes* Types, uint8 TypeCount, OutputInfo* Outputs);

		public:
			FetchShaderSourceEventHandler OnFetchShaderSourceEvent;

		private:
			ASTCompilerBase* m_Compilers[DEVICE_TYPE_COUNT];
		};
	}
}

#endif