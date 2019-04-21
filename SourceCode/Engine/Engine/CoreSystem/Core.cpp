// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <CoreSystem\Core.h>
#include <CoreSystem\Private\CoreSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\RenderingManager.h>
#include <GameObjectSystem\SceneManager.h>
#include <ResourceSystem\ResourceManager.h>
#include <FontSystem\FontManager.h>
#include <Profiler\Profiling.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace GameObjectSystem;
	using namespace Rendering;
	using namespace FontSystem;
	using namespace Utility;
	using namespace MemoryManagement::Allocator;

	namespace CoreSystem
	{
		using namespace Private;

		template<typename BaseType>
		BaseType *Allocate(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&CoreSystemAllocators::CoreSystemAllocator, sizeof(BaseType)));
		}

		template<typename BaseType>
		void Deallocate(BaseType *Ptr)
		{
			DeallocateMemory(&CoreSystemAllocators::CoreSystemAllocator, Ptr);
		}

		SINGLETON_DEFINITION(Core)

			Core::Core(void) :
			m_Windows(&CoreSystemAllocators::CoreSystemAllocator),
			m_Device(nullptr),
			m_FPS(0),
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
			RootAllocator *rootAllocator = RootAllocator::GetInstance();

			RenderingManager *rendering = RenderingManager::Create(rootAllocator);

			Assert(m_Windows.GetSize() != 0, "There's no window to Initialize");

			m_Device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);

			m_Device->SetWindow(m_Windows[0]);

			m_Device->Initialize();

			m_Device->GetDevice()->SetDebugCallback([](int32 ID, cstr Source, cstr Message, cstr Type, IDevice::Severities Severity) { Assert(false, Message); });

			printf(m_Device->GetDevice()->GetVendorName());
			printf(m_Device->GetDevice()->GetRendererName());
			printf(m_Device->GetDevice()->GetShadingLanguageVersion());

			ResourceManager::Create(rootAllocator);
			SceneManager::Create(rootAllocator);
			FontManager::Create(rootAllocator);

#if DEBUG_MODE
			Profiler::RealtimeProfiler::Create(rootAllocator);
#endif

			ResourceManager *resMgr = ResourceManager::GetInstance();
			resMgr->CheckResources();
		}

		void Core::Update(void)
		{
			static SceneManager &sceneMgr = *SceneManager::GetInstance();

			BeginProfilerFrame();

			ProfileFunction();

			PlatformWindow::PollEvents();

			Scene activeScene = sceneMgr.GetActiveScene();
			if (activeScene.IsValid())
				activeScene.Update();

			m_Device->BeginRender();

			if (activeScene.IsValid())
				activeScene.Render();

			m_Device->EndRender();

			uint64 time = HighResolutionTime::GetTime().GetMilliseconds();

			++m_FrameCount;

			if (time >= m_NextFPSCalculationTime)
			{
				m_FPS = m_FrameCount;
				m_FrameCount = 0;
				m_NextFPSCalculationTime = time + 1000;
			}

			EndProfilerFrame();
		}

		Window * Core::CreateWindow(const Vector2I &Size, const String &Title)
		{
			Window *window = CreateWindowInternal(Size, Title);

			m_Windows.Add(window);

			return window;
		}

		void Core::DestroyWindow(Window * Window)
		{
			m_Windows.Remove(Window);

			DestroyWindowInternal(Window);
		}

		Window * Core::CreateWindowInternal(const Vector2I &Size, const String & Title)
		{
			Window *window = Allocate<Window>();
			Construct(window, Title);

			window->Initialize();

			window->SetTitle(Title);
			window->SetSize(Size);

			return window;
		}

		void Core::DestroyWindowInternal(Window * Window)
		{
			Window->~Window();
			Deallocate(Window);
		}
	}
}