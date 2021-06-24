// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Pass.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
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

		bool Pass::SetProgram(ProgramResource* Program)
		{
			if (m_Program == Program)
				return true;

			m_Program = Program;

			CleanupData();

			if (m_Program->IsNull())
				return false;

			CloneData(**(*m_Program));

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

			CleanupData();

			if (!m_Program->IsNull())
				CloneData(Other);

			return *this;
		}

#undef CLEANUP_BUFFER_INFO
#undef IMPLEMENT_SET_TEXTURE
	}
}