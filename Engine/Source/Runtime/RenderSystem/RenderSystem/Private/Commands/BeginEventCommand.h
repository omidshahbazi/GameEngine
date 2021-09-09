// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BEGIN_EVENT_COMMAND_H
#define BEGIN_EVENT_COMMAND_H

#include <RenderSystem\Private\Commands\CommandBase.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderSystem
	{
		namespace Private
		{
			namespace Commands
			{
				class BeginEventCommand : public CommandBase
				{
				public:
					BeginEventCommand(const WString& Label);

					void Execute(IDevice* Device) override;

				private:
					WString m_Label;
				};
			}
		}
	}
}

#endif