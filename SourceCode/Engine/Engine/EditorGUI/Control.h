// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <Containers\ListenerContainer.h>
#include <EditorGUI\EditorRenderDeviceBase.h>

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
			friend class RenderableWindow;

		private:
			typedef Vector<Control*> ControlList;

		public:
			class IListener
			{
			public:
				virtual void OnPositionChanged(Control* Control) = 0;

				virtual void OnSizeChanged(Control* Control) = 0;

				virtual void OnKeyDown(Control* Control, PlatformWindow::VirtualKeys Key) = 0;

				virtual void OnKeyUp(Control* Control, PlatformWindow::VirtualKeys Key) = 0;

				virtual void OnKeyPressed(Control* Control, PlatformWindow::VirtualKeys Key) = 0;

				virtual void OnMouseDown(Control* Control, PlatformWindow::VirtualKeys Key, const Vector2I& Position) = 0;

				virtual void OnMouseUp(Control* Control, PlatformWindow::VirtualKeys Key, const Vector2I& Position) = 0;

				virtual void OnMouseClick(Control* Control, PlatformWindow::VirtualKeys Key, const Vector2I& Position) = 0;

				virtual void OnMouseWheel(Control* Control, const Vector2I& Position, uint16 Delta) = 0;

				virtual void OnMouseMove(Control* Control, const Vector2I& Position) = 0;

				virtual void OnMouseLeave(Control* Control) = 0;
			};

			LISTENER_DECLARATION(IListener)

		public:
			Control(void);
			virtual ~Control(void)
			{
			}

			virtual void RenderAll(EditorRenderDeviceBase* Device) const;
			virtual void Render(EditorRenderDeviceBase* Device) const = 0;

			void AddChild(Control* Control);
			void RemoveChild(Control* Control);

			void SetPosition(const Vector2I& Value);
			void SetSize(const Vector2I& Value);
			void SetRect(const RectI& Value);
			INLINE const RectI& GetRect(void) const
			{
				return m_Rect;
			}
			INLINE virtual const RectI& GetClientRect(void) const
			{
				return m_Rect;
			}
			INLINE Control* GetParent(void)
			{
				return m_Parent;
			}

		protected:
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
			virtual void OnMouseMove(const Vector2I& Position)
			{
			}
			virtual void OnMouseLeave(void)
			{
			}

		private:
			virtual void OnInternalKeyDown(PlatformWindow::VirtualKeys Key);
			virtual void OnInternalKeyUp(PlatformWindow::VirtualKeys Key);
			virtual void OnInternalKeyPressed(PlatformWindow::VirtualKeys Key);

			virtual void OnInternalMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position);
			virtual void OnInternalMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position);
			virtual void OnInternalMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position);
			virtual void OnInternalMouseWheel(const Vector2I& Position, uint16 Delta);
			virtual void OnInternalMouseMove(const Vector2I& Position);
			virtual void OnInternalMouseLeave(void);

		private:
			Control* m_Parent;
			ControlList m_Children;
			RectI m_Rect;
		};
	}
}

#endif