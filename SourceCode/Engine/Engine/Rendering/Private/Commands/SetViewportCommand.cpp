// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\SetViewportCommand.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				SetViewportCommand::SetViewportCommand(const Vector2I& Position, const Vector2I& Size) :
					m_Position(Position),
					m_Size(Size)
				{
				}

				void SetViewportCommand::Execute(IDevice* Device)
				{
					Device->SetViewport(m_Position, m_Size);
				}
			}
		}
	}
}
