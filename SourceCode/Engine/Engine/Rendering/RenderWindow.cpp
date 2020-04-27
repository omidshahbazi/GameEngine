// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\RenderWindow.h>
#include <Common\BitwiseUtils.h>

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
				SetShowFrame(false);
				SetTitleBarEnabled(false);
				SetSystemMenuEnabled(false);
				SetIsFixedSize(true);

				SetState(Window::States::Maximized);
			}
			else
			{
				SetIsTopMost(false);
				SetShowFrame(true);
				SetTitleBarEnabled(true);
				SetSystemMenuEnabled(true);
				SetIsFixedSize(false);

				SetState(Window::States::Noraml);
			}
		}
	}
}