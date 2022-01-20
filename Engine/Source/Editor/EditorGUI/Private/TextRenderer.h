// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <EditorGUI\Private\RendererBase.h>
#include <MathContainers\MathContainers.h>
#include <FontSystem\Font.h>
#include <RenderCommon\GPUAlignedType.h>

namespace Engine
{
	using namespace RenderSystem;
	using namespace RenderCommon;
	using namespace MathContainers;
	using namespace FontSystem;

	namespace EditorGUI
	{
		namespace Private
		{
			class TextRenderer : public RendererBase
			{
			private:
				struct Data
				{
				public:
					GPUAlignedVector4F FontTextureBound;
					GPUAlignedColorF Color;
				};

			public:
				TextRenderer(void);
				virtual ~TextRenderer(void)
				{
				}

				virtual void Render(EditorRenderCommandBuffer* CommandBuffer, const Vector2I& Position) const override;

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
				FontResource* m_Font;
				float32 m_Size;
				bool m_IsMultiLine;
				float32 m_LineSpacing;
				Data m_Data;

				WString m_Text;
			};
		}
	}
}

#endif