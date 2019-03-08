// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <CoreSystem\Core.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\RenderingManager.h>
#include <GameObjectSystem\SceneManager.h>
#include <ResourceSystem\ResourceManager.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace GameObjectSystem;
	using namespace Rendering;
	using namespace Utility;
	using namespace MemoryManagement::Allocator;

	namespace CoreSystem
	{
		DynamicSizeAllocator allocator("Core System Allocator", RootAllocator::GetInstance(), MegaByte);

		template<typename BaseType>
		BaseType *Allocate(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&allocator, sizeof(BaseType)));
		}

		template<typename BaseType>
		void Deallocate(BaseType *Ptr)
		{
			DeallocateMemory(&allocator, Ptr);
		}

		SINGLETON_DEFINITION(Core)

			Core::Core(void) :
			//m_Windows(&RenderingAllocators::RenderingSystemAllocator),
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
			RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());

			Assert(m_Windows.GetSize() != 0, "There's no window to Initialize");

			m_Device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);

			m_Device->SetWindow(m_Windows[0]);

			m_Device->Initialize();


			SceneManager::Create(RootAllocator::GetInstance());
			ResourceManager::Create(RootAllocator::GetInstance());
		}

		void Core::Update(void)
		{
			static SceneManager &sceneMgr = *SceneManager::GetInstance();

			PlatformWindow::PollEvents();

			Scene activeScene = sceneMgr.GetActiveScene();
			if (activeScene.IsValid())
				activeScene.Update();

			if (activeScene.IsValid())
				activeScene.Render();

			m_Device->SetRenderTarget(nullptr);
			m_Device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255));

			m_Device->BeginRender();

			m_Device->SubmitCommands();

			m_Device->EndRender();

			uint64 time = HighResolutionTime::GetTime().GetMilliseconds();

			++m_FrameCount;

			if (time >= m_NextFPSCalculationTime)
			{
				m_FPS = m_FrameCount;
				m_FrameCount = 0;
				m_NextFPSCalculationTime = time + 1000;
			}
		}

		Window * Core::CreateWindow(uint16 Width, uint16 Height, const String &Title)
		{
			Window *window = CreateWindowInternal(Width, Height, Title);

			m_Windows.Add(window);

			return window;
		}

		void Core::DestroyWindow(Window * Window)
		{
			m_Windows.Remove(Window);

			DestroyWindowInternal(Window);
		}

		Window * Core::CreateWindowInternal(uint16 Width, uint16 Height, const String & Title)
		{
			Window *window = Allocate<Window>();
			Construct(window, Title);

			window->Initialize();

			window->SetTitle(Title);
			window->SetSize(Width, Height);

			return window;
		}

		void Core::DestroyWindowInternal(Window * Window)
		{
			Window->~Window();
			Deallocate(Window);
		}
	}
}