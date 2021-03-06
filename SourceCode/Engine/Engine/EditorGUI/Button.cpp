// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Button.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	namespace EditorGUI
	{
		using namespace Private;

		Button::Button(void) :
			m_IsAutoSize(false),
			m_IsFirstTimeSet(true),
			m_NormalSprite(nullptr),
			m_HoveredSprite(nullptr),
			m_PressedSprite(nullptr),
			m_DisabledSprite(nullptr)
		{
			m_NormalSprite = m_Sprite.GetSprite();

			SetSize({ 80, 40 });
			SetDrawMode(SpriteRenderer::DrawModes::Simple);
			SetForeColor(ColorUI8::Black);
		}

		void Button::Update(void)
		{
			if (m_IsAutoSize)
			{
				auto sprite = m_Sprite.GetSprite();
				if (m_IsAutoSize && sprite != nullptr)
					m_Sprite.SetDimension((*sprite)->GetDimension());
			}

			m_Sprite.Update();
			m_Text.Update();
		}

		void Button::Render(EditorRenderDeviceBase* Device) const
		{
			auto& rect = GetRect();

			m_Sprite.Render(Device, rect.Position);

			m_Text.Render(Device, rect.Position);
		}

		void Button::OnEnabledChanged(void)
		{
			m_Sprite.SetSprite(GetIsEnabled() ? m_NormalSprite : m_DisabledSprite);
		}

		void Button::OnSizeChanged(void)
		{
			if (m_IsAutoSize)
				return;

			m_Sprite.SetDimension(GetRect().Size);
		}

		void Button::OnRotationChanged(void)
		{
			m_Sprite.SetRotation(GetRotation());
		}

		void Button::OnMouseEnter(const Vector2I& Position)
		{
			Control::OnMouseEnter(Position);

			SpriteResource* sprite = m_NormalSprite;
			if (m_HoveredSprite != nullptr)
				sprite = m_HoveredSprite;

			m_Sprite.SetSprite(sprite);
		}

		void Button::OnMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			SpriteResource* sprite = m_NormalSprite;
			if (m_PressedSprite != nullptr)
				sprite = m_PressedSprite;

			m_Sprite.SetSprite(sprite);
		}

		void Button::OnMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			OnMouseEnter(Position);

			CALL_CALLBACK(IListener, OnClicked, this);
		}

		void Button::OnMouseLeave(void)
		{
			Control::OnMouseLeave();

			m_Sprite.SetSprite(m_NormalSprite);
		}

		void Button::OnInternalClicked(void)
		{
			OnClicked();

			CALL_CALLBACK(IListener, OnClicked, this);
		}
	}
}