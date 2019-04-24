// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef INTERNAL_MODEL_PARSER_H
#define INTERNAL_MODEL_PARSER_H

#include <Utility\AssetParser\ModelParser.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		namespace AssetParser
		{
			class UTILITY_API InternalModelParser : public ModelParser
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