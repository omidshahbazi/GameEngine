// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CLEAR_COMMAND_H
#define CLEAR_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <Rendering\IDevice.h>

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
					ClearCommand(IDevice::ClearFlags Flags, Color Color);

					void Execute(IDevice *Device) override;

				private:
					IDevice::ClearFlags m_Flags;
					Color m_Color;
				};
			}
		}
	}
}

#endif