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
			Render(Device);

			auto& clientRect = GetClientRect();

			Device->SetPivot(clientRect.Position);

			for each (auto child in m_Children)
				child->RenderAll(Device);
		}

		void Control::AddChild(Control* Control)
		{
			if (Control == nullptr)
				return;

			Control->m_Parent = this;
			m_Children.Add(Control);
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

			OnPositionChanged();

			CALL_CALLBACK(IListener, OnPositionChanged, this);
		}

		void Control::SetSize(const Vector2I& Value)
		{
			if (m_Rect.Size == Value)
				return;

			m_Rect.Size = Value;

			OnSizeChanged();

			CALL_CALLBACK(IListener, OnSizeChanged, this);
		}

		void Control::SetRect(const RectI& Value)
		{
			SetPosition(Value.Position);
			SetSize(Value.Size);
		}

		void Control::OnInternalKeyDown(PlatformWindow::VirtualKeys Key)
		{
			CALL_CALLBACK(IListener, OnKeyDown, this, Key);
		}

		void Control::OnInternalKeyUp(PlatformWindow::VirtualKeys Key)
		{
			CALL_CALLBACK(IListener, OnKeyUp, this, Key);
		}

		void Control::OnInternalKeyPressed(PlatformWindow::VirtualKeys Key)
		{
			CALL_CALLBACK(IListener, OnKeyPressed, this, Key);
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

			CALL_CALLBACK(IListener, OnMouseDown, this, Key, Position);
		}

		void Control::OnInternalMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CALL_CALLBACK(IListener, OnMouseUp, this, Key, Position);
		}

		void Control::OnInternalMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CALL_CALLBACK(IListener, OnMouseClick, this, Key, Position);
		}

		void Control::OnInternalMouseWheel(const Vector2I& Position, uint16 Delta)
		{
			CALL_CALLBACK(IListener, OnMouseWheel, this, Position, Delta);
		}

		void Control::OnInternalMouseMove(const Vector2I& Position)
		{
			CALL_CALLBACK(IListener, OnMouseMove, this, Position);
		}

		void Control::OnInternalMouseLeave(void)
		{
			CALL_CALLBACK(IListener, OnMouseLeave, this);
		}
	}
}