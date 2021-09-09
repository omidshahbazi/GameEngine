// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_PARSER_H
#define MESH_PARSER_H

#include <RenderDevice\MeshInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderDevice;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API MeshParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, MeshInfo& MeshInfo);
			static void Parse(const ByteBuffer& Buffer, SubMeshInfo& SubMeshInfo);

			static uint64 GetDumpSize(const MeshInfo& MeshInfo);
			static void Dump(ByteBuffer& Buffer, const MeshInfo& MeshInfo);
		};
	}
}

#endif