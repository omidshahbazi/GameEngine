// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILTIN_ASSETS_H
#define BUILTIN_ASSETS_H

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
				static const String DEFAULT_SHADER_NAME;
				static const String DEFAULT_SHADER_SOURCE;

				static const String SPRITE_RENDERER_SHADER_NAME;
				static const String SPRITE_RENDERER_SHADER_SOURCE;

				static const WString WHITE_TEXTURE_NAME;

				static const String CUBE_MESH;

				static const String SPHERE_MESH;

				static const String CONE_MESH;
			};
		}
	}
}
#endif