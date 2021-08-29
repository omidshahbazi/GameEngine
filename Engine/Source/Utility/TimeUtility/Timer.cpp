// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <TimeUtility\Timer.h>
#include <TimeUtility\HighResolutionTime.h>

namespace Engine
{
	namespace TimeUtility
	{
		Timer::Timer(void) :
			m_IsRunning(false),
			m_FrameNumber(1),
			m_StartTime(0),
			m_Time(0),
			m_DeltaTime(0)
		{
		}

		void Timer::Start(void)
		{
			Reset();

			m_IsRunning = true;
		}

		void Timer::Pause(void)
		{
			m_IsRunning = false;
		}

		void Timer::Reset(void)
		{
			m_FrameNumber = 1;
			m_StartTime = m_Time = HighResolutionTime::GetTime().GetSeconds();
			m_DeltaTime = 0;
		}

		void Timer::Update(void)
		{
			if (!m_IsRunning)
				return;

			++m_FrameNumber;

			float32 currTime = HighResolutionTime::GetTime().GetSeconds();

			m_DeltaTime = currTime - m_Time;

			m_Time = currTime;
		}

		float32 Timer::GetRealTime(void)
		{
			return HighResolutionTime::GetTime().GetSeconds();
		}
	}
}