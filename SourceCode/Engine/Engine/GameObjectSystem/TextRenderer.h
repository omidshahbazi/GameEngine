// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <GameObjectSystem\Component.h>
#include <FontSystem\Font.h>

namespace Engine
{
	using namespace FontSystem;

	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API TextRenderer : public Component
		{
		public:
			TextRenderer(IDType SceneID, IDType ID, GameObjectTypes Type);

			void SetFont(FontHandle *Font);

			void SetText(const WString &Text);

			void SetMaterial(Material *Material);
		};
	}
}

#endif