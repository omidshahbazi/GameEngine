// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <InputSystem\KeyCodes.h>
#include <InputSystem\Private\IInputWrapper.h>

namespace Engine
{
	namespace InputSystem
	{
		class INPUTSYSTEM_API InputManager
		{
		private:
			SINGLETON_DECLARATION(InputManager)

		private:
			InputManager(void);
			~InputManager(void);

		public:
			void Update(void);

			INLINE bool GetKeyDown(KeyCodes Key)
			{
				uint16 index = (uint16)Key;

				return (!m_WasKeyDown[index] && m_IsKeyDown[index]);
			}

			INLINE bool GetKeyUp(KeyCodes Key)
			{
				uint16 index = (uint16)Key;

				return (m_WasKeyDown[index] && !m_IsKeyDown[index]);
			}

			INLINE bool GetKey(KeyCodes Key)
			{
				uint16 index = (uint16)Key;

				return m_IsKeyDown[index];
			}

		private:
			Private::IInputWrapper **m_InputWrappers;
			uint8 m_InputWrapperCount;

			bool m_WasKeyDown[(uint16)KeyCodes::COUNT];
			bool m_IsKeyDown[(uint16)KeyCodes::COUNT];
		};
	}
}

#endif