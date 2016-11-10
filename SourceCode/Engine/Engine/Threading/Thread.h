// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef THREAD_H
#define THREAD_H

namespace Engine
{
	using namespace Common;

	namespace Threading
	{
		class Thread
		{
		public:
			typedef uint32 ThreadHandle;
			typedef void(ThreadProcedure)(void *);

		public:
			Thread(uint32 StackSize);

			void Do(ThreadProcedure Procedure);

			bool IsIdle(void) const
			{
				return m_IsIdle;
			}

		private:
			static void Procedure(void *This);

		private:
			ThreadHandle m_Handle;
			bool m_IsIdle;
			ThreadProcedure *m_Procedure;
		};
	}
}

#endif