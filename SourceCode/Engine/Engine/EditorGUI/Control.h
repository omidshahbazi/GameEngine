// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <Containers\ListenerContainer.h>
#include <EditorGUI\EditorRenderDeviceBase.h>

#include <iostream>

namespace Engine
{
	namespace Rendering
	{
		class DeviceInterface;
	}

	using namespace Rendering;
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
			class IListener
			{
			public:
				virtual void OnVisibleChanged(Control* Control)
				{
				}

				virtual void OnEnabledChanged(Control* Control)
				{
				}

				virtual void OnPositionChanged(Control* Control)
				{
				}

				virtual void OnSizeChanged(Control* Control)
				{
				}

				virtual void OnKeyDown(Control* Control, PlatformWindow::VirtualKeys Key)
				{
				}

				virtual void OnKeyUp(Control* Control, PlatformWindow::VirtualKeys Key)
				{
				}

				virtual void OnKeyPressed(Control* Control, PlatformWindow::VirtualKeys Key)
				{
				}

				virtual void OnMouseDown(Control* Control, PlatformWindow::VirtualKeys Key, const Vector2I& Position)
				{
				}

				virtual void OnMouseUp(Control* Control, PlatformWindow::VirtualKeys Key, const Vector2I& Position)
				{
				}

				virtual void OnMouseClick(Control* Control, PlatformWindow::VirtualKeys Key, const Vector2I& Position)
				{
				}

				virtual void OnMouseWheel(Control* Control, const Vector2I& Position, uint16 Delta)
				{
				}

				virtual void OnMouseEnter(Control* Control, const Vector2I& Position)
				{
				}

				virtual void OnMouseMove(Control* Control, const Vector2I& Position)
				{
				}

				virtual void OnMouseLeave(Control* Control)
				{
				}
			};

			LISTENER_DECLARATION(IListener)

		public:
			Control(void);
			virtual ~Control(void)
			{
			}

			virtual void RenderAll(EditorRenderDeviceBase* Device);
			virtual void Render(EditorRenderDeviceBase* Device) = 0;

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

				CALL_CALLBACK(IListener, OnVisibleChanged, this);
			}

			INLINE bool GetIsEnabled(void)
			{
				return m_IsEnabled;
			}
			void SetIsEnabled(bool Value)
			{
				m_IsEnabled = Value;

				OnEnabledChanged();

				CALL_CALLBACK(IListener, OnEnabledChanged, this);
			}

		protected:
			virtual void RenderAll(EditorRenderDeviceBase* Device, const Vector2I& Pivot);

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

		private:
			Control* m_Parent;
			ControlList m_Children;
			RectI m_Rect;

			bool m_IsVisible;
			bool m_IsEnabled;

			Vector2I m_LastPosition;
			bool m_IsMouseOver;
		};
	}
}

#endif