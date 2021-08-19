// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef FILE_WATCHER
#define FILE_WATCHER

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace FileUtility
	{
		class FILEUTILITY_API FileWatcher
		{
		public:
			struct FileInfo
			{
			public:
				FileInfo(uint64 Microseconds) :
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

		};
	}
}

#endif