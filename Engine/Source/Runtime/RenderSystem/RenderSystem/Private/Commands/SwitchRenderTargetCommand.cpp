// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\Commands\SwitchRenderTargetCommand.h>
#include <RenderDevice\IDevice.h>
#include <RenderSystem\RenderTarget.h>

namespace Engine
{
	using namespace RenderDevice;

	namespace RenderSystem
	{
		namespace Private
		{
			namespace Commands
			{
				SwitchRenderTargetCommand::SwitchRenderTargetCommand(RenderTarget* RenderTarget) :
					m_RenderTarget(RenderTarget)
				{
				}

				void SwitchRenderTargetCommand::Execute(IDevice* Device)
				{
					Device->BindRenderTarget(m_RenderTarget == nullptr ? 0 : m_RenderTarget->GetHandle());
				}
			}
		}
	}
}
