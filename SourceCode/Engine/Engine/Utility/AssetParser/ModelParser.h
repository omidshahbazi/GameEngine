// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MODEL_PARSER_H
#define MODEL_PARSER_H

#include <Common\PrimitiveTypes.h>
#include <Rendering\MeshInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;

	namespace Utility
	{
		namespace AssetParser
		{
			class ModelParser
			{
			public:
				virtual void Parse(const byte *Data, uint32 Size, MeshInfo &MeshInfo) = 0;
				virtual void Dump(ByteBuffer &Buffer, MeshInfo &MeshInfo) = 0;
			};

		}
	}
}

#endif