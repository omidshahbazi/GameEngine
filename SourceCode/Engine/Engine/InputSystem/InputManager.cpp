// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <InputSystem\InputManager.h>
#include <Platform\PlatformMemory.h>
#include <InputSystem\Private\KeyboardWrapper.h>
#include <InputSystem\Private\InputSystemAllocators.h>

namespace Engine
{
	using namespace Platform;

	namespace InputSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(InputManager)

			InputManager::InputManager(void) :
			m_InputWrappers(nullptr),
			m_InputWrapperCount(0)
		{
#if WINDOWS || LINUX
			m_InputWrapperCount = 1;

			uint8 size = m_InputWrapperCount * sizeof(IInputWrapper*);
			m_InputWrappers = ReinterpretCast(IInputWrapper**, AllocateMemory(&InputSystemAllocators::InputSystemAllocator, size));
			PlatformMemory::Set(m_InputWrappers, 0, size);

			uint8 index = 0;

			m_InputWrappers[index] = ReinterpretCast(IInputWrapper*, AllocateMemory(&InputSystemAllocators::InputSystemAllocator, sizeof(KeyboardWrapper)));
			Construct(ReinterpretCast(KeyboardWrapper*, m_InputWrappers[index]), &m_WasKeyDown[0], &m_IsKeyDown[0]);

			++index;
#endif

			PlatformMemory::Set(m_WasKeyDown, 0, sizeof(m_WasKeyDown));
			PlatformMemory::Set(m_IsKeyDown, 0, sizeof(m_IsKeyDown));
		}

		InputManager::~InputManager(void)
		{
		}

		void InputManager::Update(void)
		{
			for (uint8 i = 0; i < m_InputWrapperCount; ++i)
				m_InputWrappers[i]->Update();
		}
	}
}