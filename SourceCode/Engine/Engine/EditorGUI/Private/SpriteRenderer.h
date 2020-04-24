// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <EditorGUI\Private\RendererBase.h>
#include <Rendering\Material.h>
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

				virtual void Render(EditorRenderDeviceBase* Device) const override;

				const ColorUI8& GetColor(void) const
				{
					return m_Color;
				}
				void SetColor(const ColorUI8& Value);

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

				const Vector2I& GetElementDimension(void) const
				{
					return m_ElementDimension2I;
				}
				void SetElementDimension(const Vector2I& Value);

			private:
				Material m_Material;
				Pass* m_Pass;

				ColorUI8 m_Color;

				DrawModes m_DrawMode;
				SpriteHandle* m_Sprite;

				Vector2I m_ElementDimension2I;
				Vector3F m_ElementDimension3F;
			};
		}
	}
}

#endif