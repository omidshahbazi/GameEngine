// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include <InputSystem\Private\IInputWrapper.h>

namespace Engine
{
	namespace InputSystem
	{
		namespace Private
		{
			class INPUTSYSTEM_API KeyboardWrapper : public IInputWrapper
			{
			public:
				KeyboardWrapper(bool *WasKeyDown, bool *IsKeyDown);
				~KeyboardWrapper(void);

			public:
				void Update(void) override;

			private:
				bool *m_WasKeyDown;
				bool *m_IsKeyDown;
			};
		}
	}
}

#endif