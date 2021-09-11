// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include <Common\PrimitiveTypes.h>
#include <RenderDevice\ProgramInfo.h>
#include <RenderDevice\CompiledProgramInfo.h>

namespace Engine
{
	using namespace Common;
	using namespace RenderDevice;

	namespace ProgramCompiler
	{
		class PROGRAMCOMPILER_API Compiler
		{
		public:
			static bool Compile(const ProgramInfo& Info, const DeviceTypes* Types, uint8 TypeCount, CompiledProgramInfo* CompiledInfos);
		};
	}
}

#endif