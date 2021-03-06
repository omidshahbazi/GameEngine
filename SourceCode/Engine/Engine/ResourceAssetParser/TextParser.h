// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_PARSER_H
#define TEXT_PARSER_H

#include <ResourceSystem\TextInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace ResourceSystem;

	namespace ResourceAssetParser
	{
		// Parse from origin file and data file are in a same way
		class RESOURCEASSETPARSER_API TextParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, TextInfo& TextInfo);

			static uint64 GetDumpSize(const TextInfo& TextInfo);
			static void Dump(ByteBuffer& Buffer, const TextInfo& TextInfo);
		};
	}
}

#endif