// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CLEAR_COMMAND_H
#define CLEAR_COMMAND_H

#include <RenderSystem\Private\Commands\CommandBase.h>
#include <RenderDevice\IDevice.h>

namespace Engine
{
	namespace RenderSystem
	{
		namespace Private
		{
			namespace Commands
			{
				class ClearCommand : public CommandBase
				{
				public:
					ClearCommand(ClearFlags Flags, const ColorUI8& Color);

					void Execute(IDevice* Device) override;

				private:
					ClearFlags m_Flags;
					ColorUI8 m_Color;
				};
			}
		}
	}
}

#endif