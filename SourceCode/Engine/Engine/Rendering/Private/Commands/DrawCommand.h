// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DRAW_COMMAND_H
#define DRAW_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace MathContainers;

	namespace ResourceSystem
	{
		template<typename T>
		class ResourceHandle;
	}

	namespace Rendering
	{
		class Mesh;
		class Shader;
		class Pass;

		namespace Private
		{
			namespace Commands
			{
				class DrawCommand : public CommandBase
				{
				public:
					DrawCommand(Mesh *Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Shader* Shader);
					DrawCommand(Mesh *Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Pass *Pass);

					void Execute(IDevice *Device) override;

				private:
					Mesh *m_Mesh;
					Matrix4F m_Model;
					Matrix4F m_View;
					Matrix4F m_Projection;
					Matrix4F m_MVP;
					Shader*m_Shader;
					Pass *m_Pass;
				};
			}
		}
	}
}

#endif