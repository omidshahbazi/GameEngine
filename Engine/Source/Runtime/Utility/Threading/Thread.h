// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_H
#define THREAD_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Platform\PlatformThread.h>
#include <Containers\Promise.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;

	namespace Threading
	{
		class THREADING_API Thread
		{
		public:
			typedef std::function<void(void*, Thread*)> ProcedureEx;

		public:
			Thread(void);
			~Thread(void);

			void Initialize(PlatformThread::Procedure Procedure, uint32 StackSize = 0, void* Arguments = nullptr, bool Suspended = false);
			Promise<void> Shutdown(bool Force = false);

			uint32 GetID(void) const;

			String GetName(void) const;
			void SetName(const String& Value);

			void Join(void);

			void Suspend(void);
			void Resume(void);

			void SetCoreAffinity(uint32 CoreIndex);

			bool GetShouldExit(void) const
			{
				return m_ShouldExit;
			}

			bool GetHasExited(void) const
			{
				return m_ExitedPromiseBlock.GetIsDone();
			}

		private:
			PlatformThread::Handle m_Handle;
			AtomicBool m_ShouldExit;
			PromiseBlock<void> m_ExitedPromiseBlock;
		};
	}
}

#endif