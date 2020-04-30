// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <EditorGUI\Private\RendererBase.h>
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

				void SetColor(const ColorUI8& Value) override;

				float GetSize(void) const
				{
					return m_Size;
				}
				void SetSize(float Value)
				{
					m_Size = Value;
				}

			private:
				Font* m_Font;

				float m_Size;
			};
		}
	}
}

#endif