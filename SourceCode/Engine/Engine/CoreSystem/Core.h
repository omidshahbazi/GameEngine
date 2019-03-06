// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_H
#define CORE_H

#include <MemoryManagement\Singleton.h>
#include <Utility\Window.h>
#include <Containers\Strings.h>

namespace Engine
{
	namespace Utility
	{
		class Window;
	}

	using namespace Containers;
	using namespace Utility;

	namespace CoreSystem
	{
		class CORESYSTEM_API Core
		{
			//typedef Vector<Window*> WindowVector;

			SINGLETON_DECLARATION(Core)

		private:
			Core(void);

		public:
			void Update(void);

			Window *CreateWindow(uint16 Width, uint16 Height, const String &Title);
			void DestroyWindow(Window *Window);
		};
	}
}

#endif