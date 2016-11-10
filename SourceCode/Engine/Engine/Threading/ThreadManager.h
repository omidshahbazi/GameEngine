// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Threading\Thread.h>

#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

namespace Engine
{
	using namespace Common;

	namespace Threading
	{
		class ThreadManager
		{
		public:
			ThreadManager(void);

		public:
			Thread &GetThread(uint8 Index)
			{
				return m_Threads[Index];
			}

			unsigned int GetCount(void) const
			{
				return m_Count;
			}

		private:
			Thread *m_Threads;
			uint8 m_Count;
		};
	}
}

#endif