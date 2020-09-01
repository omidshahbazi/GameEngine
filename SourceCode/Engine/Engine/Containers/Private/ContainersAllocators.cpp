// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Containers\Private\ContainersAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Containers
	{
		namespace Private
		{
			CREATOR_DEFINITION(ContainersAllocators);

			DynamicSizeAllocator* ContainersAllocators::DynamicStringAllocator = nullptr;
			DynamicSizeAllocator* ContainersAllocators::ConstStringAllocator = nullptr;
			DynamicSizeAllocator* ContainersAllocators::VectorAllocator = nullptr;
			DynamicSizeAllocator* ContainersAllocators::ListAllocator = nullptr;
			DynamicSizeAllocator* ContainersAllocators::MapAllocator = nullptr;

			ContainersAllocators::ContainersAllocators(void)
			{
				static DynamicSizeAllocator dynamicStringAllocator("DynamicString Allocator", RootAllocator::GetInstance(), MegaByte * 100);
				DynamicStringAllocator = &dynamicStringAllocator;

				static DynamicSizeAllocator constStringAllocator("ConstString Allocator", RootAllocator::GetInstance(), MegaByte * 100);
				ConstStringAllocator = &constStringAllocator;

				static DynamicSizeAllocator vectorAllocator("Vector Allocator", RootAllocator::GetInstance(), MegaByte * 100);
				VectorAllocator = &vectorAllocator;

				static DynamicSizeAllocator listAllocator("List Allocator", RootAllocator::GetInstance(), MegaByte * 100);
				ListAllocator = &listAllocator;

				static DynamicSizeAllocator mapAllocator("Map Allocator", RootAllocator::GetInstance(), MegaByte * 100);
				MapAllocator = &mapAllocator;
			}
		}
	}
}