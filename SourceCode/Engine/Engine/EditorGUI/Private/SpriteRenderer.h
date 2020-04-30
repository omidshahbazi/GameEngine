// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <EditorGUI\Private\RendererBase.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Rendering;
	using namespace MathContainers;

	namespace EditorGUI
	{
		namespace Private
		{
			class SpriteRenderer : public RendererBase
			{
			public:
				enum class DrawModes
				{
					Simple = 0,
					Sliced,
					Tiled
				};

			public:
				SpriteRenderer(void);
				virtual ~SpriteRenderer(void)
				{
				}

				virtual void Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const override;

				void SetColor(const ColorUI8& Value) override;

				DrawModes GetDrawMode(void) const
				{
					return m_DrawMode;
				}
				void SetDrawMode(DrawModes Value);

				SpriteHandle* GetSprite(void) const
				{
					return m_Sprite;
				}
				void SetSprite(SpriteHandle* Value);

				void SetDimension(const Vector2I& Value) override;

			private:
				DrawModes m_DrawMode;
				SpriteHandle* m_Sprite;
			};
		}
	}
}

#endif