// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Control.h>

namespace Engine
{
	namespace EditorGUI
	{
#define CHECK_RECT() if (!GetRect().Contains(Position)) return;

		Control::Control(void) :
			m_Parent(nullptr),
			m_Rect(0, 0, 1, 1)
		{
		}

		void Control::RenderAll(EditorRenderDeviceBase* Device) const
		{
			Device->SetPivot(Vector3F(m_GlobalRect.Position.X, m_GlobalRect.Position.Y, 0));

			Render(Device);

			for each (auto child in m_Children)
				child->RenderAll(Device);
		}

		void Control::AddChild(Control* Control)
		{
			if (Control == nullptr)
				return;

			Control->m_Parent = this;
			m_Children.Add(Control);

			Control->UpdateGlobalRect();
		}

		void Control::RemoveChild(Control* Control)
		{
			if (Control == nullptr)
				return;

			if (!m_Children.Contains(Control))
				return;

			Control->m_Parent = nullptr;
			m_Children.Remove(Control);
		}

		void Control::SetPosition(const Vector2I& Value)
		{
			if (m_Rect.Position == Value)
				return;

			m_Rect.Position = Value;

			UpdateGlobalRect();

			CALL_CALLBACK(IListener, OnPositionChanged, this);
		}

		void Control::SetSize(const Vector2I& Value)
		{
			if (m_Rect.Size == Value)
				return;

			m_Rect.Size = Value;

			UpdateGlobalRect();

			CALL_CALLBACK(IListener, OnSizeChanged, this);
		}

		void Control::SetRect(const RectI& Value)
		{
			SetPosition(Value.Position);
			SetSize(Value.Size);
		}

		void Control::OnPositionChanged(void)
		{
			CALL_CALLBACK(IListener, OnPositionChanged, this);
		}

		void Control::OnSizeChanged(void)
		{
			CALL_CALLBACK(IListener, OnPositionChanged, this);
		}

		void Control::OnKeyDown(PlatformWindow::VirtualKeys Key)
		{
			CALL_CALLBACK(IListener, OnKeyDown, this, Key);
		}

		void Control::OnKeyUp(PlatformWindow::VirtualKeys Key)
		{
			CALL_CALLBACK(IListener, OnKeyUp, this, Key);
		}

		void Control::OnKeyPressed(PlatformWindow::VirtualKeys Key)
		{
			CALL_CALLBACK(IListener, OnKeyPressed, this, Key);
		}

		void Control::OnMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CALL_CALLBACK(IListener, OnMouseDown, this, Key, Position);
		}

		void Control::OnMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CALL_CALLBACK(IListener, OnMouseUp, this, Key, Position);
		}

		void Control::OnMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CALL_CALLBACK(IListener, OnMouseClick, this, Key, Position);
		}

		void Control::OnMouseWheel(const Vector2I& Position, uint16 Delta)
		{
			CALL_CALLBACK(IListener, OnMouseWheel, this, Position, Delta);
		}

		void Control::OnMouseMove(const Vector2I& Position)
		{
			CALL_CALLBACK(IListener, OnMouseMove, this, Position);
		}

		void Control::OnMouseLeave(void)
		{
			CALL_CALLBACK(IListener, OnMouseLeave, this);
		}

		void Control::OnClosing(void)
		{
			CALL_CALLBACK(IListener, OnClosing, this);
		}

		void Control::OnInternalKeyDown(PlatformWindow::VirtualKeys Key)
		{
		}

		void Control::OnInternalKeyUp(PlatformWindow::VirtualKeys Key)
		{
		}

		void Control::OnInternalKeyPressed(PlatformWindow::VirtualKeys Key)
		{
		}

		void Control::OnInternalMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_RECT()

				Vector2I localPos = Position - GetRect().Position;

			for each (auto child in m_Children)
			{
				const RectI& rect = child->GetRect();

				if (!rect.Contains(localPos))
					continue;

				OnInternalMouseDown(Key, localPos);

				return;
			}

			OnMouseDown(Key, localPos);
		}

		void Control::OnInternalMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
		}

		void Control::OnInternalMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
		}

		void Control::OnInternalMouseWheel(const Vector2I& Position, uint16 Delta)
		{
		}

		void Control::OnInternalMouseMove(const Vector2I& Position)
		{
		}

		void Control::OnInternalMouseLeave(void)
		{
		}

		void Control::UpdateGlobalRect(void)
		{
			m_GlobalRect = m_Rect;

			if (m_Parent != nullptr)
				m_GlobalRect.Position += m_Parent->m_GlobalRect.Position;

			for each (auto child in m_Children)
				child->UpdateGlobalRect();
		}
	}
}