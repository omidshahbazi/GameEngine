// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <CoreSystem\Core.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>

namespace Engine
{
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

		Core::Core(void)
		{
			//m_Windows(&RenderingAllocators::RenderingSystemAllocator),

			//for each (auto item in m_Windows)
			//	DestroyWindowInternal(item);
		}

		void Core::Update(void)
		{


			//for each (auto window in m_Windows)
			//	m_Device->SwapBuffers(window->GetHandle());


			PlatformWindow::PollEvents();
		}

		Window * Core::CreateWindow(uint16 Width, uint16 Height, const String &Title)
		{
			Window *window = Allocate<Window>();
			Construct(window, Title);

			window->Initialize();

			window->SetTitle(Title);
			window->SetSize(Width, Height);

			return window;
		}

		void Core::DestroyWindow(Window * Window)
		{
		}
	}
}