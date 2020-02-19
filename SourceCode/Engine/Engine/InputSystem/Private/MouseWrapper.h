// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MOUSE_MANAGER_H
#define MOUSE_MANAGER_H

#include <InputSystem\Private\IInputWrapper.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	namespace Utility
	{
		class Window;
	}

	using namespace MathContainers;
	using namespace Utility;

	namespace InputSystem
	{
		namespace Private
		{
			class INPUTSYSTEM_API MouseWrapper : public IInputWrapper
			{
			public:
				MouseWrapper(Vector2I *Position, Window *Window);
				~MouseWrapper(void);

			public:
				void Update(void) override;

			private:
				Vector2I *m_Position;
				Window *m_Window;
			};
		}
	}
}

#endif