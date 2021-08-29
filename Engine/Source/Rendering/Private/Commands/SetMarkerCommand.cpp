// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\SetMarkerCommand.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
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
