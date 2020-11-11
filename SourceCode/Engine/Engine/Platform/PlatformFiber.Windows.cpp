// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformFiber.h>
#include <Windows.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		//FlsAlloc
		//FlsFree
		//FlsGetValue
		//FlsSetValue

		class ProcedureAsLambda
		{
			friend class PlatformFiber;

		public:
			ProcedureAsLambda(PlatformFiber::Procedure& Procedure, void* Arguments) :
				m_Procedure(Procedure),
				m_Arguments(Arguments)
			{ }

			static void Stub(void* Arguments)
			{
				ProcedureAsLambda* pThis = ReinterpretCast(ProcedureAsLambda*, Arguments);

				pThis->m_Procedure(pThis->m_Arguments);

				delete pThis;
			}

		private:
			PlatformFiber::Procedure m_Procedure;
			void* m_Arguments;
		};

		PlatformFiber::Handle PlatformFiber::Create(Procedure Procedure, uint32 StackSize, void* Arguments)
		{
			return (PlatformFiber::Handle)CreateFiber(StackSize, (LPFIBER_START_ROUTINE)ProcedureAsLambda::Stub, new ProcedureAsLambda(Procedure, Arguments));
		}

		void PlatformFiber::Delete(Handle Fiber)
		{
			DeleteFiber(Fiber);
		}

		void PlatformFiber::Switch(Handle Fiber)
		{
			SwitchToFiber(Fiber);
		}

		bool PlatformFiber::IsRunningOnFiber(void)
		{
			return IsThreadAFiber();
		}

		PlatformFiber::Handle PlatformFiber::GetCurrentFiber(void)
		{
			return (PlatformFiber::Handle)::GetCurrentFiber();
		}

		void* PlatformFiber::GetData(void)
		{
			return ReinterpretCast(ProcedureAsLambda*, GetFiberData())->m_Arguments;
		}

		PlatformFiber::Handle PlatformFiber::ConvertThreadToFiber(void* Arguments)
		{
			return (PlatformFiber::Handle)::ConvertThreadToFiber(Arguments);
		}
	}
}
#endif