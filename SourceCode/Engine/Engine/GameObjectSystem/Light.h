// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <GameObjectSystem\Component.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Light : public Component
		{
		public:
			Light(IDType SceneID, IDType ID, GameObjectTypes Type);

			void SetType(LightTypes Type);
			void SetColor(Color Color);
		};
	}
}

#endif