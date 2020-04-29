// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <EditorGUI\Private\RendererBase.h>
#include <Rendering\Material.h>
#include <MathContainers\MathContainers.h>
#include <FontSystem\Font.h>

namespace Engine
{
	using namespace Rendering;
	using namespace MathContainers;
	using namespace FontSystem;

	namespace EditorGUI
	{
		namespace Private
		{
			class TextRenderer : public RendererBase
			{
			public:
				TextRenderer(void);
				virtual ~TextRenderer(void)
				{
				}

				virtual void Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const override;

				const ColorUI8& GetColor(void) const
				{
					return m_Color;
				}
				void SetColor(const ColorUI8& Value);

			private:
				Material m_Material;
				Pass* m_Pass;

				ColorUI8 m_Color;

				Font* m_Font;
			};
		}
	}
}

#endif