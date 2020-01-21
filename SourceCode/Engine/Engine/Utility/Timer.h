// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef TIMER
#define TIMER

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Utility
	{
		class UTILITY_API Timer
		{
		public:
			Timer(void);

			void Start(void);
			void Pause(void);
			void Reset(void);

			void Update(void);

			INLINE float32 GetTime(void) const
			{
				return (m_Time - m_StartTime);
			}

			INLINE float32 GetDeltaTime(void) const
			{
				return m_DeltaTime;
			}

			static float32 GetRealTime(void);

		private:
			bool m_IsRunning;
			int32 m_FrameNumber;
			float32 m_StartTime;
			float32 m_Time;
			float32 m_DeltaTime;
		};
	}
}

#endif