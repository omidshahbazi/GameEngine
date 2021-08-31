// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILED_PROGRAM_PARSER_H
#define COMPILED_PROGRAM_PARSER_H

#include <Rendering\CompiledProgramInfo.h>
#include <Rendering\RenderingCommon.h>
#include <Rendering\ProgramInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Common;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API CompiledProgramParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, String& Source);
			static void Parse(const ByteBuffer& Buffer, DeviceTypes& DeviceType, CompiledProgramInfo& CompiledProgramInfo);

			static uint64 GetDumpSize(const String& Source);
			static uint64 GetDumpSize(const CompiledProgramInfo& CompiledProgramInfo);
			
			static void Dump(ByteBuffer& Buffer, const String& Source);
			static void Dump(ByteBuffer& Buffer, DeviceTypes DeviceType, const CompiledProgramInfo& CompiledProgramInfo);
		};
	}
}

#endif