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
				virtual void OnWindowPositioned(Window* Window) = 0;

				virtual void OnWindowResized(Window* Window) = 0;
			};

			LISTENER_DECLARATION(IListener)

		public:
			Window(const String& Name);
			~Window(void);

		public:
			bool Initialize(void);

			void SetVisible(bool Value);

			void SetTitle(const String& Value);

			const Vector2I& GetPosition(void) const
			{
				return m_Position;
			}

			void SetPosition(const Vector2I& Value);

			const Vector2I& GetClientPosition(void) const
			{
				return m_ClientPosition;
			}

			const Vector2I& GetSize(void) const
			{
				return m_Size;
			}

			void SetSize(const Vector2I& Value);

			const Vector2I& GetClientSize(void) const
			{
				return m_ClientSize;
			}

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
			Vector2I m_Size;

			//Minimize = 0x20000000L,
			//	Maximize = 0x01000000L,
			//Fullscreen
			//	DialogFrame = 0x00400000L,
			//	ThickFrame = 0x00040000L,
			bool m_IsVisible;
			bool m_IsDisabled;
			bool m_ShowCaption;
			bool m_ShowBorder;
			bool m_IsTabStop;
			bool m_ShowMinimizeBox;
			bool m_ShowMaximizeBox;

			bool m_IsTopMost;
			bool m_AcceptFiles;

			bool m_IsRightToLeft;



			//DialogModalFrame = 0x00000001L,
			//	ToolWindow = 0x00000080L,
			//	WindowEdge = 0x00000100L,
			//	ClientEdge = 0x00000200L,
			//	Right = 0x00001000L,
			//	Left = 0x00000000L,

			Vector2I m_ClientPosition;
			Vector2I m_ClientSize;

			bool m_ShouldClose;
		};
	}
}

#endif