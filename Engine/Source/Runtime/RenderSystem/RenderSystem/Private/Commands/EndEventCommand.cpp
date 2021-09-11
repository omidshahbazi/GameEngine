// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\Commands\EndEventCommand.h>
#include <RenderDevice\IDevice.h>

namespace Engine
{
	namespace RenderSystem
	{
		namespace Private
		{
			namespace Commands
			{
				EndEventCommand::EndEventCommand(void)
				{
				}

				void EndEventCommand::Execute(IDevice* Device)
				{
					Device->EndEvent();
				}
			}
		}
	}
}