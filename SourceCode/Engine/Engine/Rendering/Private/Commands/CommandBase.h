// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
					virtual void Execute(IDevice *Device) = 0;
				};
			}
		}
	}
}

#endif