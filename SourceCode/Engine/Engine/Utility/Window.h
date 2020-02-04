// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Containers\Strings.h>
#include <MathContainers\MathContainers.h>
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

			void SetVisible(bool Value);

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

			void UpdateStyle(void);

		private:
			PlatformWindow::WindowHandle m_Handle;
			PlatformWindow::Styles m_Style;

			String m_Name;
			String m_Title;
			Vector2I m_Position;
			Vector2I m_ClientPosition;
			Vector2I m_Size;
			Vector2I m_ClientSize;
			bool m_IsFullscreen;
			bool m_ShouldClose;

				Child = 0x40000000L,
				Minimize = 0x20000000L,
				Visible = 0x10000000L,
				Disabled = 0x08000000L,
				Maximize = 0x01000000L,
				Caption = 0x00C00000L,
				Border = 0x00800000L,
				DialogFrame = 0x00400000L,
				ThickFrame = 0x00040000L,
				TabStop = 0x00010000L,
				MinimizeBox = 0x00020000L,
				MaximizeBox = 0x00010000L,

		};
	}
}

#endif