// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Button.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	namespace EditorGUI
	{
		using namespace Private;

		Button::Button(void) :
			m_IsFirstTimeSet(true),
			m_NormalSprite(nullptr),
			m_HoveredSprite(nullptr),
			m_PressedSprite(nullptr)
		{
			m_NormalSprite = m_Sprite.GetSprite();

			SetDrawMode(SpriteRenderer::DrawModes::Simple);
		}

		void Button::Render(EditorRenderDeviceBase* Device) const
		{
			m_Sprite.Render(Device, GetRect().Position);
		}

		void Button::OnSizeChanged(void)
		{
			m_Sprite.SetElementDimension(GetRect().Size);
		}

		void Button::OnMouseEnter(const Vector2I& Position)
		{
			SpriteHandle* sprite = m_NormalSprite;
			if (m_HoveredSprite != nullptr)
				sprite = m_HoveredSprite;

			m_Sprite.SetSprite(sprite);
		}

		void Button::OnMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			SpriteHandle* sprite = m_NormalSprite;
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
			m_Sprite.SetSprite(m_NormalSprite);
		}

		void Button::OnInternalClicked(void)
		{
			OnClicked();

			CALL_CALLBACK(IListener, OnClicked, this);
		}
	}
}