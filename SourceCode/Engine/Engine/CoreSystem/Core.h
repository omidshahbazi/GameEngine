// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_H
#define CORE_H

#include <MemoryManagement\Singleton.h>
#include <Utility\Window.h>
#include <Containers\Strings.h>
#include <Rendering\DeviceInterface.h>

namespace Engine
{
	namespace Utility
	{
		class Window;
	}

	using namespace Rendering;
	using namespace Containers;
	using namespace Utility;

	namespace CoreSystem
	{
		class CORESYSTEM_API Core
		{
		private:
			typedef Vector<Window*> WindowVector;

			SINGLETON_DECLARATION(Core)

		private:
			Core(void);
			~Core(void);

		public:
			void Initialize(void);

			void Update(void);

			Window *CreateWindow(const Vector2I &Size, const String &Title);
			void DestroyWindow(Window *Window);

			uint32 GetFPS(void) const
			{
				return m_FPS;
			}

		private:
			Window * CreateWindowInternal(const Vector2I &Size, const String &Title);
			void DestroyWindowInternal(Window *Window);
			
		private:
			WindowVector m_Windows;

			DeviceInterface *m_Device;

			uint32 m_FPS;
			uint32 m_FrameCount;
			uint64 m_NextFPSCalculationTime;

		};
	}
}

#endif