// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILT_IN_ASSETS_H
#define BUILT_IN_ASSETS_H

#include <Containers\Strings.h>

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
				static cstr DEFAULT_PROGRAM_NAME;
				static cstr DEFAULT_PROGRAM_SOURCE;

				static cstr SPRITE_RENDERER_PROGRAM_NAME;
				static cstr SPRITE_RENDERER_PROGRAM_SOURCE;

				static cstr WHITE_TEXTURE_NAME;

				static cstr QUAD_MESH_NAME;

				static cstr CUBE_MESH_NAME;
				static cstr CUBE_MESH_DATA;

				static cstr SPHERE_MESH_NAME;
				static cstr SPHERE_MESH_DATA;

				static cstr CONE_MESH_NAME;
				static cstr CONE_MESH_DATA;
			};
		}
	}
}
#endif