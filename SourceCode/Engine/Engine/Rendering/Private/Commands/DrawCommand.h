// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DRAW_COMMAND_H
#define DRAW_COMMAND_H

#include <Rendering\Private\Commands\CommandBase.h>
#include <MathContainers\MathContainers.h>
#include <Rendering\Pass.h>

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

		namespace Private
		{
			namespace Commands
			{
				class DrawCommand : public CommandBase
				{
				public:
					DrawCommand(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program);
					DrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Pass* Pass);
					virtual ~DrawCommand(void)
					{
					}

					void Execute(IDevice* Device) override;

				private:
					Mesh* m_Mesh;
					Matrix4F m_Model;
					Matrix4F m_View;
					Matrix4F m_Projection;
					Matrix4F m_MVP;
					Program* m_Program;
					bool m_CreatedByPass;
					Program::ConstantInfoMap m_Constants;
					IDevice::State m_RenderState;
				};
			}
		}
	}
}

#endif