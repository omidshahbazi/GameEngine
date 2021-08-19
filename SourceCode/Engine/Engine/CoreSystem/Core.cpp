// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <CoreSystem\Core.h>
#include <CoreSystem\Private\CoreSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <WindowUtility\Window.h>
#include <FileUtility\FileSystem.h>
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
	using namespace WindowUtility;
	using namespace FileUtility;
	using namespace MemoryManagement::Allocator;
	using namespace Debugging;
	using namespace Profiler;

	namespace CoreSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(Core);

		Core::Core(void) :
			m_Initialized(false),
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
			if (m_Initialized)
				DeInitialize();

			FileSystem::Deinitialize();
		}

		void Core::Initialize(Info* Info)
		{
			Assert(!m_Initialized, "Core already initialized");
			Assert(Info != nullptr, "Info cannot be null");

			FileSystem::SetWorkingPath(Info->WorkingPath);

			RootAllocator* rootAllocator = RootAllocator::GetInstance();

			RenderingManager* rendering = RenderingManager::Create(rootAllocator);

			Assert(m_Windows.GetSize() != 0, "There's no window to Initialize");

			m_Device = rendering->CreateDevice(DeviceTypes::DirectX12);

			m_Device->Initialize();

			for (auto window : m_Windows)
				m_Contexts.Add(m_Device->CreateContext(window));
			m_Device->SetContext(m_Contexts[0]);

			Debug::LogInfo(m_Device->GetVersion());
			Debug::LogInfo(m_Device->GetVendorName());
			Debug::LogInfo(m_Device->GetRendererName());
			Debug::LogInfo(m_Device->GetShadingLanguageVersion());

			InputManager::Create(rootAllocator);
			ResourceManager::Create(rootAllocator);
			SceneManager::Create(rootAllocator);
			FontManager::Create(rootAllocator);
			RealtimeProfiler::Create(rootAllocator);

			ResourceManager* resMgr = ResourceManager::GetInstance();
			resMgr->GetCompiler()->CompileResources();

			InputManager* inputMgr = InputManager::GetInstance();
			inputMgr->Initialize();

			m_Timer.Start();

			m_Initialized = true;
		}

		void Core::DeInitialize(void)
		{
			Assert(m_Initialized, "Core is not initialized");

			for (auto item : m_Contexts)
				m_Device->DestroyContext(item);
			m_Contexts.Clear();

			for (auto item : m_Windows)
				DestroyWindowInternal(item);
			m_Windows.Clear();

			InputManager::Destroy();
			ResourceManager::Destroy();
			SceneManager::Destroy();
			FontManager::Destroy();
			RealtimeProfiler::Destroy();
			RenderingManager::Destroy();

			m_Initialized = false;
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

			for (auto context : m_Contexts)
			{
				m_Device->SetContext(context);

				m_Device->SetViewport(Vector2I::Zero, context->GetWindow()->GetClientSize());

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