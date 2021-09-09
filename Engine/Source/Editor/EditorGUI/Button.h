// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <EditorGUI\Control.h>
#include <EditorGUI\Private\SpriteRenderer.h>
#include <EditorGUI\Private\TextRenderer.h>
#include <RenderSystem\RenderWindow.h>

namespace Engine
{
	using namespace RenderSystem;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		class EDITORGUI_API Button : public Control
		{
		public:
			typedef Delegate<Button*> OnClickedEventHandler;

		public:
			Button(void);
			virtual ~Button(void)
			{
			}

			virtual void Update(void) override;

			virtual void Render(EditorRenderDeviceBase* Device) const override;

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

			SpriteResource* GetNormalSprite(void) const
			{
				return m_NormalSprite;
			}
			void SetNormalSprite(SpriteResource* Value)
			{
				m_NormalSprite = Value;

				if (m_IsFirstTimeSet)
				{
					m_IsFirstTimeSet = false;
					m_Sprite.SetSprite(Value);
				}
			}

			SpriteResource* GetHoveredSprite(void) const
			{
				return m_HoveredSprite;
			}
			void SetHoveredSprite(SpriteResource* Value)
			{
				m_HoveredSprite = Value;
			}

			SpriteResource* GetPressedSprite(void) const
			{
				return m_PressedSprite;
			}
			void SetPressedSprite(SpriteResource* Value)
			{
				m_PressedSprite = Value;
			}

			SpriteResource* GetDisabledSprite(void) const
			{
				return m_DisabledSprite;
			}
			void SetDisabledSprite(SpriteResource* Value)
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

		public:
			OnClickedEventHandler OnClickedEvent;

		private:
			bool m_IsAutoSize;
			SpriteRenderer m_Sprite;
			TextRenderer m_Text;

			bool m_IsFirstTimeSet;
			SpriteResource* m_NormalSprite;
			SpriteResource* m_HoveredSprite;
			SpriteResource* m_PressedSprite;
			SpriteResource* m_DisabledSprite;
		};
	}
}

#endif