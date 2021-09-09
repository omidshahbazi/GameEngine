// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\Commands\BeginEventCommand.h>
#include <RenderDevice\IDevice.h>

namespace Engine
{
	using namespace RenderDevice;

	namespace RenderSystem
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