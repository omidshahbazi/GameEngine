// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef END_EVENT_COMMAND_H
#define END_EVENT_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class EndEventCommand : public CommandBase
				{
				public:
					EndEventCommand(void);

					void Execute(IDevice* Device) override;
				};
			}
		}
	}
}

#endif