// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DRAW_COMMAND_H
#define DRAW_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <Containers\MathContainers.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class Mesh;
		class Program;

		namespace Private
		{
			namespace Commands
			{
				class DrawCommand : public CommandBase
				{
				public:
					DrawCommand(Mesh *Mesh, const Matrix4F &Transform, Program *Program) :
						m_Mesh(Mesh),
						m_Transform(Transform),
						m_Program(Program)
					{
					}

					void Execute(IDevice *Device) override;

				private:
					Mesh *m_Mesh;
					Matrix4F m_Transform;
					Program *m_Program;
				};
			}
		}
	}
}

#endif