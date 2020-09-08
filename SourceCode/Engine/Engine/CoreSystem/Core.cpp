// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <CoreSystem\Core.h>
#include <CoreSystem\Private\CoreSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>
#include <Utility\FileSystem.h>
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
	using namespace Profiler;

	namespace CoreSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(Core);

		Core::Core(void) :
			m_Windows(CoreSystemAllocators::CoreSystemAllocator),
			m_Device(nullptr),
			m_FPS(0),
			m_AverageFrameTime(0),
			m_FrameCount(0),
			m_NextFPSCalculationTime(0)
		{
			CoreSystemAllocators::Create();

			FileSystem::Initialize();
		}

		Core::~Core(void)
		{
			DeInitialize();

			FileSystem::Deinitialize();
		}

		void Core::Initialize(void)
		{
			RootAllocator* rootAllocator = RootAllocator::GetInstance();

			RenderingManager* rendering = RenderingManager::Create(rootAllocator);

			Assert(m_Windows.GetSize() != 0, "There's no window to Initialize");

			m_Device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);

			for each (auto window in m_Windows)
				m_Contexts.Add(m_Device->CreateContext(window));
			m_Device->SetContext(m_Contexts[0]);

			m_Device->Initialize();

			m_Device->GetDevice()->SetDebugCallback([](int32 ID, IDevice::DebugSources Source, cstr Message, IDevice::DebugTypes Type, IDevice::DebugSeverities Severity) { if (Type == IDevice::DebugTypes::Error) Assert(false, Message); });

			Debug::LogInfo(m_Device->GetDevice()->GetVersion());
			Debug::LogInfo(m_Device->GetDevice()->GetVendorName());
			Debug::LogInfo(m_Device->GetDevice()->GetRendererName());
			Debug::LogInfo(m_Device->GetDevice()->GetShadingLanguageVersion());

			InputManager::Create(rootAllocator);
			ResourceManager::Create(rootAllocator);
			SceneManager::Create(rootAllocator);
			FontManager::Create(rootAllocator);
			RealtimeProfiler::Create(rootAllocator);

			ResourceManager* resMgr = ResourceManager::GetInstance();
			resMgr->CheckResources();

			InputManager* inputMgr = InputManager::GetInstance();
			inputMgr->Initialize();

			m_Timer.Start();
		}

		void Core::DeInitialize(void)
		{
			for each (auto item in m_Contexts)
				m_Device->DestroyContext(item);
			m_Contexts.Clear();

			for each (auto item in m_Windows)
				DestroyWindowInternal(item);
			m_Windows.Clear();

			InputManager::Destroy();
			ResourceManager::Destroy();
			SceneManager::Destroy();
			FontManager::Destroy();
			RealtimeProfiler::Destroy();
			RenderingManager::Destroy();
		}

		void Core::Update(void)
		{
			static SceneManager& sceneMgr = *SceneManager::GetInstance();
			static InputManager& input = *InputManager::GetInstance();

			BeginProfilerFrame();

			ProfileFunction();

			for each (auto context in m_Contexts)
			{
				m_Device->SetContext(context);

				PlatformWindow::PollEvents();

				input.Update();

				Scene activeScene = sceneMgr.GetActiveScene();
				if (activeScene.IsValid())
					activeScene.Update();

				m_Device->BeginRender();

				if (activeScene.IsValid())
					activeScene.Render();

				m_Device->EndRender();
			}

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
			Window* window = CoreSystemAllocators::CoreSystemAllocator_Allocate<Window>();
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
			CoreSystemAllocators::CoreSystemAllocator_Deallocate(Window);
		}
	}
}