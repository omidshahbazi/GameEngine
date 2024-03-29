// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef HIGH_RESOLUTION_TIME
#define HIGH_RESOLUTION_TIME

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace TimeUtility
	{
		class TIMEUTILITY_API HighResolutionTime
		{
		public:
			struct Timestamp
			{
			public:
				Timestamp(uint64 Microseconds) :
					m_Microseconds(Microseconds)
				{
				}

				float64 GetSeconds(void) const
				{
					return m_Microseconds / 1000000.F;
				}

				float64 GetMilliseconds(void) const
				{
					return m_Microseconds / 1000.F;
				}

				uint64 GetMicroseconds(void) const
				{
					return m_Microseconds;
				}

			private:
				uint64 m_Microseconds;
			};

		public:
			static Timestamp GetTime(void);

		private:
			float64 m_Frequency;
		};
	}
}

#endif