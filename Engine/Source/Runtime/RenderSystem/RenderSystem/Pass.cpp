// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Pass.h>
#include <Platform\PlatformMemory.h>
#include <RenderSystem\Program.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Platform;
	using namespace Debugging;

	namespace RenderSystem
	{
		using namespace Private;

		Pass::Pass(void) :
			m_Program(nullptr),
			m_Queue(RenderQueues::Default)
		{
		}

		Pass::Pass(ProgramResource* Program) :
			m_Queue(RenderQueues::Default)
		{
			SetProgram(Program);
		}

		Pass::Pass(const Pass& Other)
		{
			*this = Other;
		}

		Pass::Pass(Pass&& Other)
		{
			*this = std::move(Other);
		}

		Pass::~Pass(void)
		{
			m_Program = nullptr;
		}

		bool Pass::SetProgram(ProgramResource* Program)
		{
			if (m_Program == Program)
				return true;

			m_Program = Program;

			if (m_Program == nullptr)
				return true;

			CoreDebugAssert(Categories::RenderSystem, !m_Program->IsNull(), "Program cannot be null to initialize a Pass");

			SyncData(*(m_Program->GetPointer()), false);

			return true;
		}

		void Pass::SetRenderState(const IDevice::State& State)
		{
			PlatformMemory::Copy(&State, &m_RenderState, 1);
		}

		Pass& Pass::operator=(const Pass& Other)
		{
			m_Program = Other.m_Program;
			m_Queue = Other.m_Queue;
			m_RenderState = Other.m_RenderState;

			SyncData(Other, true);

			return *this;
		}
	}
}