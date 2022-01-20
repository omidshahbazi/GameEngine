// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <EditorGUI\Private\RendererBase.h>
#include <MathContainers\MathContainers.h>
#include <RenderCommon\GPUAlignedType.h>

namespace Engine
{
	using namespace RenderSystem;
	using namespace RenderCommon;
	using namespace MathContainers;

	namespace EditorGUI
	{
		namespace Private
		{
			class SpriteRenderer : public RendererBase
			{
			private:
				struct Data
				{
				public:
					GPUAlignedFloat32 DrawMode;
					GPUAlignedVector2F ElementDimension;
					GPUAlignedVector2F TextureDimension;
					GPUAlignedVector4F TextureBorders;
					GPUAlignedColorF Color;
				};

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

				virtual void Update(void) override;

				virtual void Render(EditorRenderCommandBuffer* CommandBuffer, const Vector2I& Position) const override;

				void SetColor(const ColorUI8& Value) override;

				DrawModes GetDrawMode(void) const
				{
					return m_DrawMode;
				}
				void SetDrawMode(DrawModes Value);

				SpriteResource* GetSprite(void) const
				{
					return m_Sprite;
				}
				void SetSprite(SpriteResource* Value);

				void SetDimension(const Vector2I& Value) override;

			private:
				DrawModes m_DrawMode;
				SpriteResource* m_Sprite;
				bool m_IsDirty;
				Data m_Data;
			};
		}
	}
}

#endif