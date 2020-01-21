// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Containers\Strings.h>
#include <Containers\MathContainers.h>
#include <Containers\ListenerContainer.h>
#include <Platform\PlatformWindow.h>

namespace Engine
{
	using namespace Containers;
	using namespace Platform;

	namespace Utility
	{
		class UTILITY_API Window
		{
		public:
			class UTILITY_API IListener
			{
			public:
				virtual void OnWindowPositioned(Window *Window) = 0;

				virtual void OnWindowResized(Window *Window) = 0;
			};

			LISTENER_DECLARATION(IListener)

		public:
			Window(const String &Name);
			~Window(void);

		public:
			bool Initialize(void);

			void SetTitle(const String &Value);

			const Vector2I &GetPosition(void) const
			{
				return m_Position;
			}

			void SetPosition(const Vector2I &Value);

			const Vector2I &GetClientPosition(void) const
			{
				return m_ClientPosition;
			}

			const Vector2I &GetSize(void) const
			{
				return m_Size;
			}

			void SetSize(const Vector2I &Value);

			const Vector2I &GetClientSize(void) const
			{
				return m_ClientSize;
			}

			bool IsFullscreen(void) const
			{
				return m_IsFullscreen;
			}

			void SetFullscreen(bool Value);

			INLINE bool ShouldClose(void) const
			{
				return m_ShouldClose;
			}

			INLINE PlatformWindow::WindowHandle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			bool MessageProcedure(PlatformWindow::WindowMessages Message);

		private:
			PlatformWindow::WindowHandle m_Handle;
			String m_Name;
			String m_Title;
			Vector2I m_Position;
			Vector2I m_ClientPosition;
			Vector2I m_Size;
			Vector2I m_ClientSize;
			bool m_IsFullscreen;
			bool m_ShouldClose;
		};
	}
}

#endif