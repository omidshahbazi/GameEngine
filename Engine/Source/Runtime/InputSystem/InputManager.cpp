// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <InputSystem\InputManager.h>
#include <Platform\PlatformMemory.h>
#include <InputSystem\Private\KeyboardWrapper.h>
#include <InputSystem\Private\MouseWrapper.h>
#include <InputSystem\Private\InputSystemAllocators.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\RenderContext.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderSystem;
	using namespace Debugging;

	namespace InputSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(InputManager);

		InputManager::InputManager(void) :
			m_Initialized(false),
			m_InputWrappers(nullptr),
			m_InputWrapperCount(0)
		{
			InputSystemAllocators::Create();
		}

		InputManager::~InputManager(void)
		{
			if (m_InputWrappers != nullptr)
			{
				InputSystemAllocators::InputSystemAllocator_Deallocate(m_InputWrappers[0]);
				InputSystemAllocators::InputSystemAllocator_Deallocate(m_InputWrappers[1]);
				InputSystemAllocators::InputSystemAllocator_Deallocate(m_InputWrappers);
			}
		}

		void InputManager::Initialize(void)
		{
			CoreDebugAssert(Categories::RenderSystem, !m_Initialized, "InputManager already initialized");

			m_Window = RenderManager::GetInstance()->GetDevice()->GetContext()->GetWindow();

#if WINDOWS || LINUX
			m_InputWrapperCount = 2;

			uint8 size = m_InputWrapperCount * sizeof(IInputWrapper*);
			m_InputWrappers = InputSystemAllocators::InputSystemAllocator_AllocateArray<IInputWrapper*>(size);
			PlatformMemory::Set(m_InputWrappers, 0, size);

			uint8 index = 0;

			m_InputWrappers[index] = InputSystemAllocators::InputSystemAllocator_Allocate<KeyboardWrapper>();
			Construct(ReinterpretCast(KeyboardWrapper*, m_InputWrappers[index]), &m_WasKeyDown[0], &m_IsKeyDown[0]);
			++index;

			m_InputWrappers[index] = InputSystemAllocators::InputSystemAllocator_Allocate<MouseWrapper>();
			Construct(ReinterpretCast(MouseWrapper*, m_InputWrappers[index]), &m_MousePosition, m_Window);
			++index;
#endif

			PlatformMemory::Set(m_WasKeyDown, 0, sizeof(m_WasKeyDown));
			PlatformMemory::Set(m_IsKeyDown, 0, sizeof(m_IsKeyDown));

			m_Initialized = true;
		}

		void InputManager::Update(void)
		{
			for (uint8 i = 0; i < m_InputWrapperCount; ++i)
				m_InputWrappers[i]->Update();
		}
	}
}