// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILT_IN_ASSETS_H
#define BUILT_IN_ASSETS_H

#include <Containers\GUID.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		namespace Private
		{
			class BuiltInAssets
			{
			public:
				static GUID DEFAULT_PROGRAM_GUID;
				static cstr DEFAULT_PROGRAM_SOURCE;

				static GUID SPRITE_RENDERER_PROGRAM_GUID;
				static cstr SPRITE_RENDERER_PROGRAM_SOURCE;

				static GUID WHITE_TEXTURE_GUID;

				static GUID QUAD_MESH_GUID;

				static GUID CUBE_MESH_GUID;
				static cstr CUBE_MESH_DATA;

				static GUID SPHERE_MESH_GUID;
				static cstr SPHERE_MESH_DATA;

				static GUID CONE_MESH_GUID;
				static cstr CONE_MESH_DATA;
			};
		}
	}
}
#endif