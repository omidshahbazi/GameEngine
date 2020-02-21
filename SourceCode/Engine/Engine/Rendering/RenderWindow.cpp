// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\RenderWindow.h>

namespace Engine
{
	namespace Rendering
	{
		RenderWindow::RenderWindow(const String& Name) :
			Window(Name)
		{
			Initialize();
			SetIsVisible(true);
		}

		void RenderWindow::SetIsFullscreen(bool Value)
		{
			if (Value)
			{
				SetIsTopMost(true);
				SetState(Window::States::Maximized);
				SetShowCaption(false);

				//WS_THICKFRAME

				//	WS_EX_DLGMODALFRAME |
				//	WS_EX_WINDOWEDGE |
				//	WS_EX_CLIENTEDGE |
				//	WS_EX_STATICEDGE)
			}
			else
			{
				SetIsTopMost(false);
				SetState(Window::States::Noraml);
				SetShowCaption(true);
			}
		}
	}
}