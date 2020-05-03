// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_H
#define CORE_H

#include <Common\Definitions.h>
#include <MemoryManagement\Singleton.h>
#include <Utility\Window.h>
#include <Utility\Timer.h>
#include <Containers\Strings.h>
#include <Rendering\DeviceInterface.h>

//TODO: fully cleanup and restart
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
		WRAPPER_OBJECT()
		class CORESYSTEM_API Core
		{
		private:
			typedef Vector<Window*> WindowVector;
			typedef Vector<RenderContext*> ContextVector;

			SINGLETON_DECLARATION(Core)

		private:
			Core(void);
			~Core(void);

		public:
			void Initialize(void);

			void Update(void);

			Window* CreateWindow(const Vector2I& Size, const String& Title);
			void DestroyWindow(Window* Window);

			INLINE const Timer& GetTimer(void) const
			{
				return m_Timer;
			}

			INLINE uint32 GetFPS(void) const
			{
				return m_FPS;
			}

			INLINE float32 GetAverageFrameTime(void) const
			{
				return m_AverageFrameTime;
			}

		private:
			Window* CreateWindowInternal(const Vector2I& Size, const String& Title);
			void DestroyWindowInternal(Window* Window);

		private:
			WindowVector m_Windows;
			ContextVector m_Contexts;

			DeviceInterface* m_Device;

			Timer m_Timer;
			uint32 m_FPS;
			float32 m_AverageFrameTime;
			uint32 m_FrameCount;
			uint32 m_NextFPSCalculationTime;
		};
	}
}

#endif