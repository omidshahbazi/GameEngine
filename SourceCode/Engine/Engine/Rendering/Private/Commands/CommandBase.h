// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMMAND_BASE_H
#define COMMAND_BASE_H

namespace Engine
{
	namespace Rendering
	{
		class IDevice;

		namespace Private
		{
			namespace Commands
			{
				class CommandBase
				{
				public:
					virtual ~CommandBase(void)
					{
					}

					virtual void Execute(IDevice* Device) = 0;
				};
			}
		}
	}
}

#endif