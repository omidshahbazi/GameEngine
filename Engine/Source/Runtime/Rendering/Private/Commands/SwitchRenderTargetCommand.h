// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SWITCH_RENDER_TARGET_COMMAND_H
#define SWITCH_RENDER_TARGET_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>

namespace Engine
{
	namespace Rendering
	{
		class RenderTarget;

		namespace Private
		{
			namespace Commands
			{
				class SwitchRenderTargetCommand : public CommandBase
				{
				public:
					SwitchRenderTargetCommand(RenderTarget* RenderTarget);

					void Execute(IDevice* Device) override;

				private:
					RenderTarget* m_RenderTarget;
				};
			}
		}
	}
}

#endif