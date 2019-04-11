// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef GAME_OBJECT_SYSTEM_COMMON_H
#define GAME_OBJECT_SYSTEM_COMMON_H

namespace Engine
{
	namespace GameObjectSystem
	{
		enum class GameObjectTypes
		{
			None = 0,
			Camera,
			Renderable,
			Light
		};

		enum class ProjectionTypes
		{
			Perspective = 0,
			Orthographic
		};

		enum class LightTypes
		{
			Ambient = 0,
			Directional,
			Point,
			Spot
		};
	}
}

#endif