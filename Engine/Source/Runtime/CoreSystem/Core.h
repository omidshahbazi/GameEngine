// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_H
#define CORE_H

#include <MemoryManagement\Singleton.h>
#include <WindowUtility\Window.h>
#include <TimeUtility\Timer.h>
#include <Containers\Strings.h>
#include <RenderSystem\DeviceInterface.h>

namespace Engine
{
	namespace WindowUtility
	{
		class Window;
	}

	using namespace Containers;
	using namespace WindowUtility;
	using namespace TimeUtility;
	using namespace RenderSystem;

	namespace CoreSystem
	{
		class CORESYSTEM_API Core
		{
		private:
			typedef Vector<Window*> WindowVector;
			typedef Vector<RenderContext*> ContextVector;

			SINGLETON_DECLARATION(Core)

		public:
			struct Info
			{
			public:
				cwstr WorkingPath;
			};

		private:
			Core(void);
			~Core(void);

		public:
			void Initialize(Info* Info);
			void DeInitialize(void);

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
			bool m_Initialized;

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