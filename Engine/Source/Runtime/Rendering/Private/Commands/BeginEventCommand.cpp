// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\BeginEventCommand.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				BeginEventCommand::BeginEventCommand(const WString& Label) :
					m_Label(Label)
				{
				}

				void BeginEventCommand::Execute(IDevice* Device)
				{
					Device->BeginEvent(m_Label.GetValue());
				}
			}
		}
	}
}
