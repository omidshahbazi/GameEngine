// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_PARSER_H
#define PROGRAM_PARSER_H

#include <RenderDevice\ProgramInfo.h>
#include <RenderDevice\CompiledProgramInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Common;
	using namespace RenderDevice;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API ProgramParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, ProgramInfo& ProgramInfo);

			static uint64 GetDumpSize(const ProgramInfo& ProgramInfo);
			static void Dump(ByteBuffer& Buffer, const ProgramInfo& ProgramInfo);
		};
	}
}

#endif