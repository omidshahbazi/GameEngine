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
					DrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, const ProgramConstantHolder* ConstantsHolder);

					virtual void Execute(IDevice* Device) override;

				private:
					Mesh* m_Mesh;
					Matrix4F m_Model;
					Matrix4F m_View;
					Matrix4F m_Projection;
					Matrix4F m_MVP;
					Program* m_Program;
					ProgramConstantHolder::BufferDataBaseMap m_Buffers;
					ProgramConstantHolder::TextureDataBaseMap m_Textures;
				};

				class PassDrawCommand : public DrawCommand
				{
				public:
					PassDrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Pass* Pass) :
						DrawCommand(Allocator, Mesh, Model, View, Projection, MVP, Pass->GetProgram()->GetPointer(), Pass),
						m_RenderState(Pass->GetRenderState())
					{
					}

					void Execute(IDevice* Device) override
					{
						Device->SetState(m_RenderState);

						DrawCommand::Execute(Device);
					}

				private:
					IDevice::State m_RenderState;
				};
			}
		}
	}
}

#endif