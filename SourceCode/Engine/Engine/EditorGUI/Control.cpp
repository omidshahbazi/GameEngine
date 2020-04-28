// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Control.h>

#include <iostream>

namespace Engine
{
	namespace EditorGUI
	{
#define CHECK_RECT() if (!GetRect().Contains(Position)) return false;

#define CHECK_IS_ACTIVE() if (!m_IsVisible || !m_IsEnabled) return false;

#define GET_LOCAL_POSITION() Position - GetClientRect().Position

		Control::Control(void) :
			m_Parent(nullptr),
			m_Rect(0, 0, 1, 1),
			m_IsVisible(true),
			m_IsEnabled(true),
			m_IsMouseOver(false)
		{
		}

		void Control::RenderAll(EditorRenderDeviceBase* Device)
		{
			if (!m_IsVisible)
				return;

			Render(Device);

			auto& clientRect = GetClientRect();

			Vector2I prevPivot = Device->GetPivot();

			Device->SetPivot(clientRect.Position);

			for each (auto child in m_Children)
			{
				child->RenderAll(Device);
			}

			Device->SetPivot(prevPivot);
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

		//TODO: handle key input
		bool Control::OnInternalKeyDown(PlatformWindow::VirtualKeys Key)
		{
			CHECK_IS_ACTIVE()

				OnKeyDown(Key);
			CALL_CALLBACK(IListener, OnKeyDown, this, Key);

			return true;
		}

		bool Control::OnInternalKeyUp(PlatformWindow::VirtualKeys Key)
		{
			CHECK_IS_ACTIVE()

				OnKeyUp(Key);
			CALL_CALLBACK(IListener, OnKeyUp, this, Key);

			return true;
		}

		bool Control::OnInternalKeyPressed(PlatformWindow::VirtualKeys Key)
		{
			CHECK_IS_ACTIVE()

				OnKeyPressed(Key);
			CALL_CALLBACK(IListener, OnKeyPressed, this, Key);

			return true;
		}

		bool Control::OnInternalMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for each (auto child in m_Children)
			{
				if (child->OnInternalMouseDown(Key, localPositon))
					return true;
			}

			OnMouseDown(Key, localPositon);

			CALL_CALLBACK(IListener, OnMouseDown, this, Key, localPositon);

			return true;
		}

		bool Control::OnInternalMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for each (auto child in m_Children)
			{
				if (child->OnInternalMouseUp(Key, localPositon))
					return true;
			}

			OnMouseUp(Key, localPositon);

			CALL_CALLBACK(IListener, OnMouseUp, this, Key, localPositon);

			return true;
		}

		bool Control::OnInternalMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for each (auto child in m_Children)
			{
				if (child->OnInternalMouseClick(Key, localPositon))
					return true;
			}

			OnMouseClick(Key, localPositon);

			CALL_CALLBACK(IListener, OnMouseClick, this, Key, localPositon);

			return true;
		}

		bool Control::OnInternalMouseWheel(const Vector2I& Position, uint16 Delta)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for each (auto child in m_Children)
			{
				if (child->OnInternalMouseWheel(localPositon, Delta))
					return true;
			}

			OnMouseWheel(localPositon, Delta);

			CALL_CALLBACK(IListener, OnMouseWheel, this, localPositon, Delta);

			return true;
		}


		bool Control::OnInternalMouseEnter(const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				if (m_IsMouseOver)
					return false;

			m_IsMouseOver = true;

			OnMouseEnter(Position);
			CALL_CALLBACK(IListener, OnMouseEnter, this, Position);

			return true;
		}

		bool Control::OnInternalMouseMove(const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				if (m_LastPosition == Position)
					return false;

			m_LastPosition = Position;

			if (m_IsMouseOver && !GetRect().Contains(Position))
			{
				OnInternalMouseLeave();

				return false;
			}

			CHECK_RECT()

				OnInternalMouseEnter(Position);

			Vector2I localPositon = GET_LOCAL_POSITION();

			bool handled = false;
			for each (auto child in m_Children)
			{
				if (child->OnInternalMouseMove(localPositon))
					handled = true;
			}

			if (handled)
				return true;

			OnMouseMove(localPositon);
			CALL_CALLBACK(IListener, OnMouseMove, this, localPositon);

			return true;
		}

		bool Control::OnInternalMouseLeave(void)
		{
			CHECK_IS_ACTIVE()

				if (!m_IsMouseOver)
					return false;

			for each (auto child in m_Children)
				child->OnInternalMouseLeave();

			m_IsMouseOver = false;

			OnMouseLeave();
			CALL_CALLBACK(IListener, OnMouseLeave, this);

			return true;
		}
	}
}