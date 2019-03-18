// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DRAW_COMMAND_H
#define DRAW_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <Containers\MathContainers.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		template<typename T>
		class ResourceHandle;
	}

	namespace Rendering
	{
		class Mesh;
		class Program;
		class Pass;

		namespace Private
		{
			namespace Commands
			{
				class DrawCommand : public CommandBase
				{
				public:
					DrawCommand(Mesh *Mesh, const Matrix4F &Transform, Program *Program);
					DrawCommand(Mesh *Mesh, const Matrix4F &Transform, Pass *Pass);

					void Execute(IDevice *Device) override;

				private:
					Mesh *m_Mesh;
					Matrix4F m_Transform;
					Program *m_Program;
					Pass *m_Pass;
				};
			}
		}
	}
}

#endif