// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <InputSystem\Private\MouseWrapper.h>
#include <Platform\PlatformInput.h>
#include <Utility\Window.h>

namespace Engine
{
	using namespace Platform;
	using namespace Utility;

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

				PlatformInput::GetMousePosition(pos.X, pos.Y);

				auto &position = m_Window->GetClientPosition();
				auto &clientSize = m_Window->GetClientSize();

				pos.X -= position.X;
				pos.Y -= position.Y;

				pos.X = Mathematics::Clamp(pos.X, 0, clientSize.X);
				pos.Y = Mathematics::Clamp(pos.Y, 0, clientSize.Y);
			}
		}
	}
}