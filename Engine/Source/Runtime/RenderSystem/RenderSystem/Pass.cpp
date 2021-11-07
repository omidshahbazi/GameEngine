// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Pass.h>
#include <Platform\PlatformMemory.h>
#include <RenderSystem\Program.h>

namespace Engine
{
	using namespace Platform;

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

			UpdateData(m_Program);

			return false;
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

			UpdateData(m_Program);

			return *this;
		}

		void Pass::UpdateData(ProgramResource* Program)
		{
			if (Program->IsNull())
				Program->Then([&, Program]() { SyncData(*(Program->GetPointer())); });
			else
				SyncData(*(Program->GetPointer()));
		}
	}
}