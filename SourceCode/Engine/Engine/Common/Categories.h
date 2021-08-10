// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CATEGORIES_H
#define CATEGORIES_H

namespace Engine
{
	namespace Common
	{
		enum class Categories
		{
			None = 0,
			Default = 1,
			CoreSystem = (Default << 1),
			EditorGUI = (CoreSystem << 1),
			FontSystem = (EditorGUI << 1),
			GameObjectSystem = (FontSystem << 1),
			InputSystem = (GameObjectSystem << 1),
			Parallelizing = (InputSystem << 1),
			Rendering = (Parallelizing << 1),
			ResourceSystem = (Rendering << 1),
			All = 0xFFFFFFF
		};
	}
}

#endif