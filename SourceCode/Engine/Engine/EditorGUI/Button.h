// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <EditorGUI\Control.h>
#include <EditorGUI\Private\SpriteRenderer.h>
#include <EditorGUI\Private\TextRenderer.h>
#include <Rendering\RenderWindow.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		class EDITORGUI_API Button : public Control
		{
		public:
			class IListener : public Control::IListener
			{
			public:
				virtual void OnClicked(Button* Button)
				{
				}
			};

			LISTENER_DECLARATION(IListener)

		public:
			Button(void);
			virtual ~Button(void)
			{
			}

			virtual void Render(EditorRenderDeviceBase* Device) override;

		protected:
			virtual void OnEnabledChanged(void) override;
			virtual void OnSizeChanged(void) override;
			virtual void OnRotationChanged(void) override;

			virtual void OnMouseEnter(const Vector2I& Position) override;
			virtual void OnMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position) override;
			virtual void OnMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position) override;
			virtual void OnMouseLeave(void) override;

			virtual void OnClicked(void)
			{
			}

		public:
			const ColorUI8& GetBackColor(void) const
			{
				return m_Sprite.GetColor();
			}
			void SetBackColor(const ColorUI8& Value)
			{
				m_Sprite.SetColor(Value);
			}

			SpriteRenderer::DrawModes GetDrawMode(void) const
			{
				return m_Sprite.GetDrawMode();
			}
			void SetDrawMode(SpriteRenderer::DrawModes Value)
			{
				m_Sprite.SetDrawMode(Value);
			}

			bool GetIsAutoSize(void) const
			{
				return m_IsAutoSize;
			}
			void SetIsAutoSize(bool Value)
			{
				m_IsAutoSize = Value;
			}

			SpriteHandle* GetNormalSprite(void) const
			{
				return m_NormalSprite;
			}
			void SetNormalSprite(SpriteHandle* Value)
			{
				m_NormalSprite = Value;

				if (m_IsFirstTimeSet)
				{
					m_IsFirstTimeSet = false;
					m_Sprite.SetSprite(Value);
				}
			}

			SpriteHandle* GetHoveredSprite(void) const
			{
				return m_HoveredSprite;
			}
			void SetHoveredSprite(SpriteHandle* Value)
			{
				m_HoveredSprite = Value;
			}

			SpriteHandle* GetPressedSprite(void) const
			{
				return m_PressedSprite;
			}
			void SetPressedSprite(SpriteHandle* Value)
			{
				m_PressedSprite = Value;
			}

			SpriteHandle* GetDisabledSprite(void) const
			{
				return m_DisabledSprite;
			}
			void SetDisabledSprite(SpriteHandle* Value)
			{
				m_DisabledSprite = Value;
			}

			const ColorUI8& GetForeColor(void) const
			{
				return m_Text.GetColor();
			}
			void SetForeColor(const ColorUI8& Value)
			{
				m_Text.SetColor(Value);
			}

			float32 GetTextSize(void) const
			{
				return m_Text.GetSize();
			}
			void SetTextSize(float32 Value)
			{
				m_Text.SetSize(Value);
			}

			bool GetIsMultiLine(void) const
			{
				return m_Text.GetIsMultiLine();
			}
			void SetIsMultiLine(bool Value)
			{
				m_Text.SetIsMultiLine(Value);
			}

			float32 GetLineSpacing(void) const
			{
				return m_Text.GetLineSpacing();
			}
			void SetLineSpacing(float32 Value)
			{
				m_Text.SetLineSpacing(Value);
			}

			const WString& GetText(void) const
			{
				return m_Text.GetText();
			}
			void SetText(const String& Value)
			{
				m_Text.SetText(Value);
			}
			void SetText(const WString& Value)
			{
				m_Text.SetText(Value);
			}

		private:
			void OnInternalClicked(void);

		private:
			bool m_IsAutoSize;
			SpriteRenderer m_Sprite;
			TextRenderer m_Text;

			bool m_IsFirstTimeSet;
			SpriteHandle* m_NormalSprite;
			SpriteHandle* m_HoveredSprite;
			SpriteHandle* m_PressedSprite;
			SpriteHandle* m_DisabledSprite;
		};
	}
}

#endif