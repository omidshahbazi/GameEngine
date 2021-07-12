// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMMON_H
#define COMMON_H

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				enum class Stages
				{
					Vertex = 0,
					Tessellation,
					Geometry,
					Fragment,
					Compute
				};
			}
		}
	}
}

#endif