// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Control.h>
#include <EditorGUI\Private\EditorGUIAllocators.h>

namespace Engine
{
	namespace EditorGUI
	{
		using namespace Private;

#define CHECK_RECT() if (!GetRect().Contains(Position)) return false;

#define CHECK_IS_ACTIVE() if (!m_IsVisible || !m_IsEnabled) return false;

#define GET_LOCAL_POSITION() Position - GetClientRect().Position

		Control::Control(void) :
			m_Parent(nullptr),
			m_Rect(0, 0, 1, 1),
			m_Rotation(0),
			m_IsVisible(true),
			m_IsEnabled(true),
			m_IsMouseOver(false)
		{
		}

		Control::~Control(void)
		{
			for (auto child : m_Children)
				EditorGUIAllocators::TypesAllocator_TryDeallocate(child);
		}

		void Control::UpdateAll(void)
		{
			Update();

			for (auto child : m_Children)
				child->UpdateAll();
		}

		void Control::RenderAll(EditorRenderCommandBuffer* CommandBuffer) const
		{
			RenderAll(CommandBuffer, Vector2I::Zero);
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

			OnPositionChangedEvent(this);
		}

		void Control::SetSize(const Vector2I& Value)
		{
			if (m_Rect.Size == Value)
				return;

			m_Rect.Size = Value;

			OnSizeChanged();

			OnSizeChangedEvent(this);
		}

		void Control::SetRotation(float32 Value)
		{
			if (m_Rotation == Value)
				return;

			m_Rotation = Value;

			OnRotationChanged();

			OnRotationChangedEvent(this);
		}

		void Control::RenderAll(EditorRenderCommandBuffer* CommandBuffer, const Vector2I& Pivot) const
		{
			if (!m_IsVisible)
				return;

			Render(CommandBuffer);

			auto& clientRect = GetClientRect();

			Vector2I pivot = Pivot + clientRect.Position;

			CommandBuffer->SetPivot(pivot);

			for (auto child : m_Children)
				child->RenderAll(CommandBuffer, pivot);

			CommandBuffer->SetPivot(Pivot);
		}

		bool Control::OnInternalKeyDown(PlatformWindow::VirtualKeys Key)
		{
			CHECK_IS_ACTIVE()

				OnKeyDown(Key);
			OnKeyDownEvent(this, Key);

			return true;
		}

		bool Control::OnInternalKeyUp(PlatformWindow::VirtualKeys Key)
		{
			CHECK_IS_ACTIVE()

				OnKeyUp(Key);
			OnKeyUpEvent(this, Key);

			return true;
		}

		bool Control::OnInternalKeyPressed(PlatformWindow::VirtualKeys Key)
		{
			CHECK_IS_ACTIVE()

				OnKeyPressed(Key);
			OnKeyPressedEvent(this, Key);

			return true;
		}

		bool Control::OnInternalMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for (auto child : m_Children)
			{
				if (child->OnInternalMouseDown(Key, localPositon))
					return true;
			}

			OnMouseDown(Key, localPositon);

			OnMouseDownEvent(this, Key, localPositon);

			return true;
		}

		bool Control::OnInternalMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for (auto child : m_Children)
			{
				if (child->OnInternalMouseUp(Key, localPositon))
					return true;
			}

			OnMouseUp(Key, localPositon);

			OnMouseUpEvent(this, Key, localPositon);

			return true;
		}

		bool Control::OnInternalMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for (auto child : m_Children)
			{
				if (child->OnInternalMouseClick(Key, localPositon))
					return true;
			}

			OnMouseClick(Key, localPositon);

			OnMouseClickEvent(this, Key, localPositon);

			return true;
		}

		bool Control::OnInternalMouseWheel(const Vector2I& Position, uint16 Delta)
		{
			CHECK_IS_ACTIVE()

				CHECK_RECT()

				Vector2I localPositon = GET_LOCAL_POSITION();

			for (auto child : m_Children)
			{
				if (child->OnInternalMouseWheel(localPositon, Delta))
					return true;
			}

			OnMouseWheel(localPositon, Delta);

			OnMouseWheelEvent(this, localPositon, Delta);

			return true;
		}


		bool Control::OnInternalMouseEnter(const Vector2I& Position)
		{
			CHECK_IS_ACTIVE()

				if (m_IsMouseOver)
					return false;

			m_IsMouseOver = true;

			OnMouseEnter(Position);
			OnMouseEnterEvent(this, Position);

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
			for (auto child : m_Children)
			{
				if (child->OnInternalMouseMove(localPositon))
					handled = true;
			}

			if (handled)
				return true;

			OnMouseMove(localPositon);
			OnMouseMoveEvent(this, localPositon);

			return true;
		}

		bool Control::OnInternalMouseLeave(void)
		{
			CHECK_IS_ACTIVE()

				if (!m_IsMouseOver)
					return false;

			for (auto child : m_Children)
				child->OnInternalMouseLeave();

			m_IsMouseOver = false;

			OnMouseLeave();
			OnMouseLeaveEvent(this);

			return true;
		}
	}
}