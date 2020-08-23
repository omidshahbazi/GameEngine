// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef EDITOR_GUI_ALLOCATORS_H
#define EDITOR_GUI_ALLOCATORS_H
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace EditorGUI
	{
		namespace Private
		{
			class EditorGUIAllocators
			{
			public:
				static DynamicSizeAllocator TypesAllocator;
				DEFINE_ALLOCATOR_HELPERS(TypesAllocator);
			};
		}
	}
}
#endif