// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CLEAR_COMMAND_H
#define CLEAR_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class ClearCommand : public CommandBase
				{
				public:
					ClearCommand(IDevice::ClearFlags Flags, const ColorUI8& Color);

					void Execute(ThreadedDevice* Device) override;

				private:
					IDevice::ClearFlags m_Flags;
					ColorUI8 m_Color;
				};
			}
		}
	}
}

#endif