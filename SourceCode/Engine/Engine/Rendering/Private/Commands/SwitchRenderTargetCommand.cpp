// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\SwitchRenderTargetCommand.h>
#include <Rendering\IDevice.h>

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

				void SwitchRenderTargetCommand::Execute(IDevice * Device)
				{
					Device->BindRenderTarget(m_RenderTarget == nullptr ? 0 : m_RenderTarget->GetHandle());

					Device->SetClearColor(m_RenderTarget == nullptr ? Color(0, 255, 0, 255) : Color(255, 0, 0, 255));
					Device->Clear();
				}
			}
		}
	}
}
