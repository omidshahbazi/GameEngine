// Copyright 2016-2020 ?????????????. All Rights Reserved.
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

			void SetType(LightTypes Value);
			void SetColor(const ColorUI8& Value);
			void SetStrength(float32 Value);
			void SetRadius(float32 Value);
			void SetConstantAttenuation(float32 Value);
			void SetLinearAttenuation(float32 Value);
			void SetQuadraticAttenuation(float32 Value);
			void SetInnerCutOff(float32 Value);
			void SetOuterCutOff(float32 Value);
		};
	}
}

#endif