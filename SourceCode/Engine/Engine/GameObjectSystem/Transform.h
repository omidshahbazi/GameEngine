// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <GameObjectSystem\Component.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Transform : public Component
		{
		public:
			Transform(IDType SceneID, IDType ID, GameObjectTypes Type);

			void SetPosition(const Vector3F &Value);
			void SetRotation(const Vector3F &Value);
			void SetForward(const Vector3F &Value);
			void SetScale(const Vector3F &Value);
		};
	}
}

#endif