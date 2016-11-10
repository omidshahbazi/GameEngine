// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Threading\ThreadManager.h>
#include <Platform\MultiThreading.h>
#include <Platform\Memory.h>
#include <memory>

#define STACK_SIZE 1024 * 1024 * 1024

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		ThreadManager::ThreadManager(void) :
			m_Threads(nullptr),
			m_Count(0)
		{
			m_Count = MultiThreading::GetHardwareConcurrency();
			m_Count = 2;
			m_Threads = (Thread*)Memory::Allocate(m_Count * sizeof(Thread*));

			for (uint8 i = 0; i < m_Count; ++i)
			{
				Thread *thread = new (m_Threads + i) Thread(STACK_SIZE);

				thread->SetCoreAffinity(i);
			}
		}
	}
}