// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <InputSystem\Private\MouseWrapper.h>
#include <WindowUtility\Window.h>

namespace Engine
{
	using namespace Platform;
	using namespace WindowUtility;

	namespace InputSystem
	{
		namespace Private
		{
			MouseWrapper::MouseWrapper(Vector2I *Position, Window *Window) :
				m_Position(Position),
				m_Window(Window)
			{
			}

			MouseWrapper::~MouseWrapper(void)
			{
			}

			void MouseWrapper::Update(void)
			{
				Vector2I &pos = *m_Position;

				PlatformWindow::GetMousePosition(pos.X, pos.Y);

				auto &position = m_Window->GetClientPosition();
				auto &clientSize = m_Window->GetClientSize();

				pos.X -= position.X;
				pos.Y -= position.Y;

				pos.X = Math::Clamp(pos.X, 0, clientSize.X);
				pos.Y = Math::Clamp(pos.Y, 0, clientSize.Y);
			}
		}
	}
}