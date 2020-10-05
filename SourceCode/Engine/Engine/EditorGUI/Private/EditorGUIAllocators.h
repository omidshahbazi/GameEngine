// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef EDITOR_GUI_ALLOCATORS_H
#define EDITOR_GUI_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
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
				CREATOR_DECLARATION(EditorGUIAllocators);

			private:
				EditorGUIAllocators(void);
				~EditorGUIAllocators(void);

			public:
				STATIC_DYNAMIC_ALLOCATOR_HELPERS_DEFINITION(TypesAllocator);

			public:
				static DynamicSizeAllocator* TypesAllocator;
			};
		}
	}
}
#endif