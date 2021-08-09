// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\EndEventCommand.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
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