// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <Utility\AssetParser\ModelParser.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		namespace AssetParser
		{
			class UTILITY_API OBJParser : public ModelParser
			{
			public:
				void Parse(const ByteBuffer &Buffer, MeshInfo &MeshInfo) override;
				void Parse(const ByteBuffer &Buffer, SubMeshInfo &SubMeshInfo) override;

				void Dump(ByteBuffer &Buffer, MeshInfo &MeshInfo) override;
			};
		}
	}
}

#endif