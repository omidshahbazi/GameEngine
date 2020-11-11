// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef EDITOR_GUI_MANAGER_H
#define EDITOR_GUI_MANAGER_H

#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace EditorGUI
	{
		class EDITORGUI_API EditorGUIManager
		{
			SINGLETON_DECLARATION(EditorGUIManager);

		private:
			EditorGUIManager(void);
			virtual ~EditorGUIManager(void);
		};
	}
}

#endif