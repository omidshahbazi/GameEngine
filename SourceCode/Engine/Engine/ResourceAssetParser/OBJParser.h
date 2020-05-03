// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <ResourceAssetParser\ModelParser.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API OBJParser : public ModelParser
		{
		public:
			void Parse(const ByteBuffer& Buffer, MeshInfo& MeshInfo) override;
			void Parse(const ByteBuffer& Buffer, SubMeshInfo& SubMeshInfo) override;

			uint64 GetDumpSize(MeshInfo& MeshInfo) override
			{
				return 0;
			}

			void Dump(ByteBuffer& Buffer, MeshInfo& MeshInfo) override;
		};
	}
}

#endif