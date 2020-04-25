// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Control.h>

namespace Engine
{
	namespace EditorGUI
	{
#define CHECK_RECT() if (!GetRect().Contains(Position)) return;

#define BEGIN_HIREARCHY_MOUSE_EVENT() \
		Vector2I __LocalPosition = Position - GetClientRect().Position; \
		for each (auto child in m_Children) \
		{ \
			const RectI& rect = child->GetRect(); \
			if (!rect.Contains(__LocalPosition)) \
			{ \
				if (child->m_IsMouseOver) \
				{ \
					OnMouseLeave(); \
					CALL_CALLBACK(IListener, OnMouseLeave, this); \
				} \
				continue; \
			}

#define END_HIREARCHY_MOUSE_EVENT() \
			return; \
		}

		Control::Control(void) :
			m_Parent(nullptr),
			m_Rect(0, 0, 1, 1),
			m_IsMouseOver(false)
		{
		}

		void Control::RenderAll(EditorRenderDeviceBase* Device) const
		{
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

				m_IsMouseOver = false;

			BEGIN_HIREARCHY_MOUSE_EVENT()
				OnInternalMouseDown(Key, __LocalPosition);
			END_HIREARCHY_MOUSE_EVENT()

				OnMouseDown(Key, __LocalPosition);

			CALL_CALLBACK(IListener, OnMouseDown, this, Key, __LocalPosition);
		}

		void Control::OnInternalMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_RECT();

			BEGIN_HIREARCHY_MOUSE_EVENT()
				OnInternalMouseUp(Key, __LocalPosition);
			END_HIREARCHY_MOUSE_EVENT()

				OnMouseUp(Key, __LocalPosition);

			CALL_CALLBACK(IListener, OnMouseUp, this, Key, __LocalPosition);
		}

		void Control::OnInternalMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_RECT();

			BEGIN_HIREARCHY_MOUSE_EVENT()
				OnInternalMouseClick(Key, __LocalPosition);
			END_HIREARCHY_MOUSE_EVENT()

				OnMouseClick(Key, __LocalPosition);

			CALL_CALLBACK(IListener, OnMouseClick, this, Key, __LocalPosition);
		}

		void Control::OnInternalMouseWheel(const Vector2I& Position, uint16 Delta)
		{
			CHECK_RECT();

			BEGIN_HIREARCHY_MOUSE_EVENT()
				OnInternalMouseWheel(__LocalPosition, Delta);
			END_HIREARCHY_MOUSE_EVENT()

				OnMouseWheel(__LocalPosition, Delta);

			CALL_CALLBACK(IListener, OnMouseWheel, this, __LocalPosition, Delta);
		}

		void Control::OnInternalMouseMove(const Vector2I& Position)
		{
			BEGIN_HIREARCHY_MOUSE_EVENT()
				OnInternalMouseMove(__LocalPosition);
			END_HIREARCHY_MOUSE_EVENT()

				m_IsMouseOver = true;

			OnMouseMove(__LocalPosition);

			CALL_CALLBACK(IListener, OnMouseMove, this, __LocalPosition);
		}
	}
}