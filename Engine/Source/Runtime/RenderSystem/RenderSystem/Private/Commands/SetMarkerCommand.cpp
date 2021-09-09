// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\Commands\SetMarkerCommand.h>
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
				SetMarkerCommand::SetMarkerCommand(const WString& Label) :
					m_Label(Label)
				{
				}

				void SetMarkerCommand::Execute(IDevice* Device)
				{
					Device->SetMarker(m_Label.GetValue());
				}
			}
		}
	}
}
