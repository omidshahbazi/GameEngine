// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INTERNAL_MODEL_PARSER_H
#define INTERNAL_MODEL_PARSER_H

#include <ResourceAssetParser\ModelParser.h>

namespace Engine
{
	using namespace Containers;
	using namespace Common;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API InternalModelParser : public ModelParser
		{
		public:
			void Parse(const ByteBuffer& Buffer, MeshInfo& MeshInfo) override;
			void Parse(const ByteBuffer& Buffer, SubMeshInfo& SubMeshInfo) override;

			uint64 GetDumpSize(MeshInfo& MeshInfo) override;
			void Dump(ByteBuffer& Buffer, MeshInfo& MeshInfo) override;
		};
	}
}

#endif