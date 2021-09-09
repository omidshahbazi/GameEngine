// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_BYTE_CODE_COMPILER_H
#define I_BYTE_CODE_COMPILER_H

#include <ProgramCompilerCommon\Common.h>
#include <RenderCommon\Enums.h>

namespace Engine
{
	using namespace RenderCommon;

	namespace ProgramCompilerCommon
	{
		class IByteCodeCompiler
		{
		public:
			virtual void Compile(DeviceTypes Client, Stages Stage, cstr Source, cstr EntryPointName, bool DebugMode, byte* ByteCode, uint16& Size) = 0;
		};
	}
}

#endif