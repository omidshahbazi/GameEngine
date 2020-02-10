#include <Utility\Window.h>
#include <Platform\PlatformWindow.h>

using namespace Engine::Utility;
using namespace Engine::Platform;

void main()
{
	Window window("Test");
	window.Initialize();
	window.SetMinimumSize({ 800, 600 });
	window.SetMaximumSize({ 100024, 768000 });
	window.SetSize({ 1800, 600 });

	window.SetIsVisible(true);

	window.SetState(Window::States::Maximized);

	window.SetShowMaximizeBox(false);

	while (!window.ShouldClose())
	{
		PlatformWindow::PollEvents();
	}
}