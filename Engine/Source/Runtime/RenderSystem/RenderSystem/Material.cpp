// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Material.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ConstantBuffer.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <ProgramCompilerCommon\OutputInfo.h>
#include <Platform\PlatformMemory.h>
#include <DataUtility\Hash.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace RenderCommon::Private;
	using namespace ProgramCompilerCommon;
	using namespace Platform;
	using namespace DataUtility;
	using namespace Debugging;

	namespace RenderSystem
	{
		Material::Material(void) :
			m_Queue(RenderQueues::Default)
		{
		}

		Material::Material(ProgramResource* Program) :
			ProgramInstance(Program),
			m_Queue(RenderQueues::Default)
		{
		}

		Material::Material(const Material& Other)
		{
			*this = Other;
		}

		void Material::SetRenderState(const RenderState& State)
		{
			PlatformMemory::Copy(&State, &m_RenderState, 1);
		}

		Material& Material::operator=(const Material& Other)
		{
			m_Queue = Other.m_Queue;
			m_RenderState = Other.m_RenderState;

			ProgramInstance::operator=(Other);

			return *this;
		}
	}
}