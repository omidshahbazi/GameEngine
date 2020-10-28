// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <Rendering\ShaderInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Common;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API ShaderParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, ShaderInfo& ShaderInfo);

			static uint64 GetDumpSize(const ShaderInfo& ShaderInfo);
			static void Dump(ByteBuffer& Buffer, const ShaderInfo& ShaderInfo);
		};
	}
}

#endif