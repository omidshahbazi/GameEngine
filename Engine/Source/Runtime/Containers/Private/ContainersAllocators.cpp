// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Containers\Private\ContainersAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace Containers
	{
		namespace Private
		{
			CREATOR_DEFINITION(ContainersAllocators);

			ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocators::DynamicStringAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocators::ConstStringAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocators::VectorAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocators::ListAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocators::StackAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocators::QueueAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocators::MapAllocator = nullptr;

			ContainersAllocators::ContainersAllocators(void)
			{
				static ThreadSafeAllocator<DynamicSizeAllocator> dynamicStringAllocator("DynamicString Allocator", RootAllocator::GetInstance());
				DynamicStringAllocator = &dynamicStringAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> constStringAllocator("ConstString Allocator", RootAllocator::GetInstance());
				ConstStringAllocator = &constStringAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> vectorAllocator("Vector Allocator", RootAllocator::GetInstance());
				VectorAllocator = &vectorAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> listAllocator("List Allocator", RootAllocator::GetInstance());
				ListAllocator = &listAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> stackAllocator("Stack Allocator", RootAllocator::GetInstance());
				StackAllocator = &stackAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> queueAllocator("Queue Allocator", RootAllocator::GetInstance());
				QueueAllocator = &queueAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> mapAllocator("Map Allocator", RootAllocator::GetInstance());
				MapAllocator = &mapAllocator;
			}
		}
	}
}