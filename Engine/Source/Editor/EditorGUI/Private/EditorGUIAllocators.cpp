// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\EditorGUIAllocators.h>
#include <EditorGUI\Private\Resources.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace EditorGUI
	{
		namespace Private
		{
			CREATOR_DEFINITION(EditorGUIAllocators);

			DynamicSizeAllocator* EditorGUIAllocators::TypesAllocator = nullptr;

			EditorGUIAllocators::EditorGUIAllocators(void)
			{
				static DynamicSizeAllocator typesAllocator("EditorGUI Allocator", RootAllocator::GetInstance(), 16 * MegaByte);
				TypesAllocator = &typesAllocator;
			}

			EditorGUIAllocators::~EditorGUIAllocators(void)
			{
				Resources::Destroy();
			}
		}
	}
}