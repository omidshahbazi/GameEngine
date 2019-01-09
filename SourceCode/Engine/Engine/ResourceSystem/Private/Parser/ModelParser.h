// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MODEL_PARSER_H
#define MODEL_PARSER_H

#include <Common\PrimitiveTypes.h>
#include <Rendering\MeshInfo.h>

namespace Engine
{
	using namespace Common;
	using namespace Rendering;

	namespace ResourceSystem
	{
		namespace Private
		{
			namespace Parser
			{
				class ModelParser
				{
				public:
					virtual void Parse(const byte *Data, MeshInfo &MeshInfo) = 0;
				};
			}
		}
	}
}

#endif