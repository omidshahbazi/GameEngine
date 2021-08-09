// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SET_VIEWPORT_COMMAND_H
#define SET_VIEWPORT_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace MathContainers;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class SetViewportCommand : public CommandBase
				{
				public:
					SetViewportCommand(const Vector2I& Position, const Vector2I& Size);

					void Execute(IDevice* Device) override;

				private:
					Vector2I m_Position;
					Vector2I m_Size;
				};
			}
		}
	}
}

#endif