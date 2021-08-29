// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SET_MARKER_COMMAND_H
#define SET_MARKER_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class SetMarkerCommand : public CommandBase
				{
				public:
					SetMarkerCommand(const WString& Label);

					void Execute(IDevice* Device) override;

				private:
					WString m_Label;
				};
			}
		}
	}
}

#endif