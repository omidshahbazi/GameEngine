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
					DrawCommand(Mesh *Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Program *Program);
					DrawCommand(Mesh *Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Pass *Pass);

					void Execute(IDevice *Device) override;

				private:
					Mesh *m_Mesh;
					Matrix4F m_Model;
					Matrix4F m_View;
					Matrix4F m_Projection;
					Matrix4F m_MVP;
					Program *m_Program;
					Pass *m_Pass;
				};
			}
		}
	}
}

#endif