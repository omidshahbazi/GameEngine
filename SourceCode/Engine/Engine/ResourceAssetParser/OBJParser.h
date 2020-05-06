// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <Rendering\MeshInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API OBJParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, MeshInfo& MeshInfo);
			static void Parse(const ByteBuffer& Buffer, SubMeshInfo& SubMeshInfo);

			static void Dump(ByteBuffer& Buffer, const  MeshInfo& MeshInfo);
		};
	}
}

#endif