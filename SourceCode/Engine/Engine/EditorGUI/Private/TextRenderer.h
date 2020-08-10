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

				float32 GetSize(void) const
				{
					return m_Size;
				}
				void SetSize(float32 Value)
				{
					m_Size = Value;
				}

				bool GetIsMultiLine(void) const
				{
					return m_IsMultiLine;
				}
				void SetIsMultiLine(bool Value)
				{
					m_IsMultiLine = Value;
				}

				float32 GetLineSpacing(void) const
				{
					return m_LineSpacing;
				}
				void SetLineSpacing(float32 Value)
				{
					m_LineSpacing = Value;
				}

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

			private:
				Font* m_Font;
				float32 m_Size;
				bool m_IsMultiLine;
				float32 m_LineSpacing;

				WString m_Text;
			};
		}
	}
}

#endif