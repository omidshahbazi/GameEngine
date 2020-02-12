// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <InputSystem\KeyCodes.h>
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
		class INPUTSYSTEM_API InputManager
		{
		private:
			SINGLETON_DECLARATION(InputManager)

		private:
			InputManager(void);
			~InputManager(void);

		public:
			void Initialize(void);

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

			INLINE const Vector2I &GetMousePosition(void) const
			{
				return m_MousePosition;
			}

		private:
			Window * m_Window;

			Private::IInputWrapper **m_InputWrappers;
			uint8 m_InputWrapperCount;

			bool m_WasKeyDown[(uint16)KeyCodes::COUNT];
			bool m_IsKeyDown[(uint16)KeyCodes::COUNT];

			Vector2I m_MousePosition;
		};
	}
}

#endif