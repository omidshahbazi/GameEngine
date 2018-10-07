// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef HASH_H
#define HASH_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Utility
	{
		class UTILITY_API HighResolutionTime
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