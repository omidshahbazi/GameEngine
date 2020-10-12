// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\SwitchRenderTargetCommand.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				SwitchRenderTargetCommand::SwitchRenderTargetCommand(RenderTarget *RenderTarget) :
					m_RenderTarget(RenderTarget)
				{
				}

				void SwitchRenderTargetCommand::Execute(ThreadedDevice* Device)
				{
					Device->BindRenderTarget(m_RenderTarget == nullptr ? 0 : m_RenderTarget->GetHandle());
				}
			}
		}
	}
}
