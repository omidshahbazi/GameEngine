// Copyright 2016-2020 ?????????????. All Rights Reserved.
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

			void SetFont(FontResource* Font);

			void SetMaterial(Material* Material);

			void SetText(const String& Text)
			{
				SetText(Text.ChangeType<WString::CharType>());
			}
			void SetText(const WString& Text);

			void SetRightToLeft(bool RightToLeft);

			void SetSize(float32 Size);

			void SetOutlineThicknes(float32 OutlineThickness);
		};
	}
}

#endif