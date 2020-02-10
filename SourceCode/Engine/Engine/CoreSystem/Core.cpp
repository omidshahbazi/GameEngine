// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <CoreSystem\Core.h>
#include <CoreSystem\Private\CoreSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>
#include <Rendering\RenderingManager.h>
#include <InputSystem\InputManager.h>
#include <GameObjectSystem\SceneManager.h>
#include <ResourceSystem\ResourceManager.h>
#include <FontSystem\FontManager.h>
#include <Profiler\Profiling.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace GameObjectSystem;
	using namespace Rendering;
	using namespace InputSystem;
	using namespace FontSystem;
	using namespace Utility;
	using namespace MemoryManagement::Allocator;
	using namespace Debugging;

	namespace CoreSystem
	{
		using namespace Private;

		template<typename BaseType>
		BaseType* Allocate(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&CoreSystemAllocators::CoreSystemAllocator, sizeof(BaseType)));
		}

		template<typename BaseType>
		void Deallocate(BaseType* Ptr)
		{
			DeallocateMemory(&CoreSystemAllocators::CoreSystemAllocator, Ptr);
		}

		SINGLETON_DEFINITION(Core)

			Core::Core(void) :
			m_Windows(&CoreSystemAllocators::CoreSystemAllocator),
			m_Device(nullptr),
			m_FPS(0),
			m_AverageFrameTime(0),
			m_FrameCount(0),
			m_NextFPSCalculationTime(0)
		{
		}

		Core::~Core(void)
		{
			for each (auto item in m_Windows)
				DestroyWindowInternal(item);
			m_Windows.Clear();
		}

		void Core::Initialize(void)
		{
			RootAllocator* rootAllocator = RootAllocator::GetInstance();

			RenderingManager* rendering = RenderingManager::Create(rootAllocator);

			Assert(m_Windows.GetSize() != 0, "There's no window to Initialize");

			m_Device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);

			m_Device->SetWindow(m_Windows[0]);

			m_Device->Initialize();

			m_Device->GetDevice()->SetDebugCallback([](int32 ID, cstr Source, cstr Message, cstr Type, IDevice::Severities Severity) { Assert(false, Message); });

			Debug::LogInfo(m_Device->GetDevice()->GetVersion());
			Debug::LogInfo(m_Device->GetDevice()->GetVendorName());
			Debug::LogInfo(m_Device->GetDevice()->GetRendererName());
			Debug::LogInfo(m_Device->GetDevice()->GetShadingLanguageVersion());

			InputManager::Create(rootAllocator)->Initialize();
			ResourceManager::Create(rootAllocator);
			SceneManager::Create(rootAllocator);
			FontManager::Create(rootAllocator);

#if DEBUG_MODE
			Profiler::RealtimeProfiler::Create(rootAllocator);
#endif

			ResourceManager* resMgr = ResourceManager::GetInstance();
			resMgr->CheckResources();

			InputManager* inputMgr = InputManager::GetInstance();
			inputMgr->Initialize();

			m_Timer.Start();
		}

		void Core::Update(void)
		{
			static SceneManager& sceneMgr = *SceneManager::GetInstance();
			static InputManager& input = *InputManager::GetInstance();

			BeginProfilerFrame();

			ProfileFunction();

			PlatformWindow::PollEvents();

			input.Update();

			Scene activeScene = sceneMgr.GetActiveScene();
			if (activeScene.IsValid())
				activeScene.Update();

			m_Device->BeginRender();

			if (activeScene.IsValid())
				activeScene.Render();

			m_Device->EndRender();

			m_Timer.Update();
			uint64 time = m_Timer.GetTime();

			++m_FrameCount;

			if (time >= m_NextFPSCalculationTime)
			{
				m_FPS = m_FrameCount;
				m_AverageFrameTime = 1000.F / m_FPS;
				m_FrameCount = 0;
				++m_NextFPSCalculationTime;
			}

			EndProfilerFrame();
		}

		Window* Core::CreateWindow(const Vector2I& Size, const String& Title)
		{
			Window* window = CreateWindowInternal(Size, Title);

			m_Windows.Add(window);

			return window;
		}

		void Core::DestroyWindow(Window* Window)
		{
			m_Windows.Remove(Window);

			DestroyWindowInternal(Window);
		}

		Window* Core::CreateWindowInternal(const Vector2I& Size, const String& Title)
		{
			Window* window = Allocate<Window>();
			Construct(window, Title);

			window->Initialize();

			window->SetTitle(Title);
			window->SetMaximumSize({ 999999, 999999 });
			window->SetSize(Size);
			window->SetIsVisible(true);

			return window;
		}

		void Core::DestroyWindowInternal(Window* Window)
		{
			Window->~Window();
			Deallocate(Window);
		}
	}
}