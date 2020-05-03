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

				const WString& GetText(void) const
				{
					return m_Text;
				}
				void SetText(const String& Value)
				{
					m_Text = Value.ChangeType<char16>();
				}
				void SetText(const WString& Value)
				{
					m_Text = Value;
				}

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

				WString m_Text;

				float m_Size;
			};
		}
	}
}

#endif