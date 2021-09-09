// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <Containers\Delegate.h>
#include <EditorGUI\EditorRenderDeviceBase.h>

#include <iostream>

namespace Engine
{
	namespace RenderSystem
	{
		class DeviceInterface;
	}

	using namespace RenderSystem;
	using namespace Containers;
	using namespace MathContainers;

	namespace EditorGUI
	{
		class RenderableWindow;

		class EDITORGUI_API Control
		{
		private:
			typedef Vector<Control*> ControlList;

		public:
			typedef Delegate<Control*> VisibleChangedEventHandler;
			typedef Delegate<Control*> EnabledChangedEventHandler;
			typedef Delegate<Control*> PositionChangedEventHandler;
			typedef Delegate<Control*> SizeChangedEventHandler;
			typedef Delegate<Control*> RotationChangedEventHandler;
			typedef Delegate<Control*, PlatformWindow::VirtualKeys> KeyDownEventHandler;
			typedef Delegate<Control*, PlatformWindow::VirtualKeys> KeyUpEventHandler;
			typedef Delegate<Control*, PlatformWindow::VirtualKeys> KeyPressedEventHandler;
			typedef Delegate<Control*, PlatformWindow::VirtualKeys, const Vector2I&> MouseDownEventHandler;
			typedef Delegate<Control*, PlatformWindow::VirtualKeys, const Vector2I&> MouseUpEventHandler;
			typedef Delegate<Control*, PlatformWindow::VirtualKeys, const Vector2I&> MouseClickEventHandler;
			typedef Delegate<Control*, const Vector2I&, uint16> MouseWheelEventHandler;
			typedef Delegate<Control*, const Vector2I&> MouseEnterEventHandler;
			typedef Delegate<Control*, const Vector2I&> MouseMoveEventHandler;
			typedef Delegate<Control*> MouseLeaveEventHandler;

		public:
			Control(void);
			virtual ~Control(void);

			virtual void UpdateAll(void);
			virtual void Update(void)
			{
			}

			virtual void RenderAll(EditorRenderDeviceBase* Device) const;
			virtual void Render(EditorRenderDeviceBase* Device) const
			{
			}

			void AddChild(Control* Control);
			void RemoveChild(Control* Control);

			INLINE Control* GetParent(void)
			{
				return m_Parent;
			}

			virtual void SetPosition(const Vector2I& Value);
			virtual const Vector2I& GetPosition(void) const
			{
				return m_Rect.Position;
			}

			virtual void SetSize(const Vector2I& Value);
			virtual const Vector2I& GetSize(void) const
			{
				return m_Rect.Size;
			}

			virtual const RectI& GetRect(void) const
			{
				return m_Rect;
			}

			virtual void SetRotation(float32 Value);
			virtual float32 GetRotation(void) const
			{
				return m_Rotation;
			}

			INLINE virtual const RectI& GetClientRect(void) const
			{
				return m_Rect;
			}

			INLINE bool GetIsVisible(void)
			{
				return m_IsVisible;
			}
			void SetIsVisible(bool Value)
			{
				m_IsVisible = Value;

				OnVisibleChanged();

				OnVisibleChangedEvent(this);
			}

			INLINE bool GetIsEnabled(void)
			{
				return m_IsEnabled;
			}
			void SetIsEnabled(bool Value)
			{
				m_IsEnabled = Value;

				OnEnabledChanged();

				OnEnabledChangedEvent(this);
			}

		protected:
			virtual void RenderAll(EditorRenderDeviceBase* Device, const Vector2I& Pivot) const;

			virtual void OnVisibleChanged(void)
			{
			}
			virtual void OnEnabledChanged(void)
			{
			}

			virtual void OnPositionChanged(void)
			{
			}
			virtual void OnSizeChanged(void)
			{
			}
			virtual void OnRotationChanged(void)
			{
			}

			virtual void OnKeyDown(PlatformWindow::VirtualKeys Key)
			{
			}
			virtual void OnKeyUp(PlatformWindow::VirtualKeys Key)
			{
			}
			virtual void OnKeyPressed(PlatformWindow::VirtualKeys Key)
			{
			}

			virtual void OnMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
			{
			}
			virtual void OnMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
			{
			}
			virtual void OnMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
			{
			}
			virtual void OnMouseWheel(const Vector2I& Position, uint16 Delta)
			{
			}
			virtual void OnMouseEnter(const Vector2I& Position)
			{
			}
			virtual void OnMouseMove(const Vector2I& Position)
			{
			}
			virtual void OnMouseLeave(void)
			{
			}

			bool OnInternalKeyDown(PlatformWindow::VirtualKeys Key);
			bool OnInternalKeyUp(PlatformWindow::VirtualKeys Key);
			bool OnInternalKeyPressed(PlatformWindow::VirtualKeys Key);

			bool OnInternalMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position);
			bool OnInternalMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position);
			bool OnInternalMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position);
			bool OnInternalMouseWheel(const Vector2I& Position, uint16 Delta);
			bool OnInternalMouseEnter(const Vector2I& Position);
			bool OnInternalMouseMove(const Vector2I& Position);
			bool OnInternalMouseLeave(void);

		public:
			VisibleChangedEventHandler OnVisibleChangedEvent;
			EnabledChangedEventHandler OnEnabledChangedEvent;
			PositionChangedEventHandler OnPositionChangedEvent;
			SizeChangedEventHandler OnSizeChangedEvent;
			RotationChangedEventHandler OnRotationChangedEvent;
			KeyDownEventHandler OnKeyDownEvent;
			KeyUpEventHandler OnKeyUpEvent;
			KeyPressedEventHandler OnKeyPressedEvent;
			MouseDownEventHandler OnMouseDownEvent;
			MouseUpEventHandler OnMouseUpEvent;
			MouseClickEventHandler OnMouseClickEvent;
			MouseWheelEventHandler OnMouseWheelEvent;
			MouseEnterEventHandler OnMouseEnterEvent;
			MouseMoveEventHandler OnMouseMoveEvent;
			MouseLeaveEventHandler OnMouseLeaveEvent;

		private:
			Control* m_Parent;
			ControlList m_Children;
			RectI m_Rect;
			float32 m_Rotation;

			bool m_IsVisible;
			bool m_IsEnabled;

			Vector2I m_LastPosition;
			bool m_IsMouseOver;
		};
	}
}

#endif