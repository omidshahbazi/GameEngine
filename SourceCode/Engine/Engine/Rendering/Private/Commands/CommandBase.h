// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMMAND_BASE_H
#define COMMAND_BASE_H

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			class ThreadedDevice;

			namespace Commands
			{
				class CommandBase
				{
				public:
					virtual ~CommandBase(void)
					{
					}

					virtual void Execute(ThreadedDevice* Device) = 0;
				};
			}
		}
	}
}

#endif