// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_PARSER_H
#define PROGRAM_PARSER_H

#include <Rendering\ProgramInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Common;
	using namespace Rendering;

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