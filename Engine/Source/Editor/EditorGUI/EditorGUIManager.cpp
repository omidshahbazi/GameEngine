// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\EditorGUIManager.h>
#include <EditorGUI\Private\EditorGUIAllocators.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	namespace EditorGUI
	{
		using namespace Private;

		SINGLETON_DEFINITION(EditorGUIManager);

		EditorGUIManager::EditorGUIManager(void)
		{
			EditorGUIAllocators::Create();
			Resources::Create(EditorGUIAllocators::TypesAllocator);
		}

		EditorGUIManager::~EditorGUIManager(void)
		{
			Resources::Destroy();
		}
	}
}