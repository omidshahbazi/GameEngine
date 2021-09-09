// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GLSL_COMPILER_H
#define GLSL_COMPILER_H

#include <ProgramCompilerCommon\IByteCodeCompiler.h>

namespace Engine
{
	using namespace Common;
	using namespace ProgramCompilerCommon;

	namespace GLSLCompiler
	{
		namespace Private
		{
			class GLSLCompiler : public IByteCodeCompiler
			{
			public:
				void Initialize(void);
				void Uninitialize(void);

				void Compile(DeviceTypes Client, Stages Stage, cstr Source, cstr EntryPointName, bool DebugMode, byte* ByteCode, uint16& Size) override;
			};
		}
	}
}

#endif