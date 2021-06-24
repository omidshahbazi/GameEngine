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
				class DrawCommandBase : public CommandBase
				{
				public:
					DrawCommandBase(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, const ProgramConstantHolder::BufferDataMap& Buffers, const ProgramConstantHolder::TextureDataMap& Textures);

					virtual void Execute(IDevice* Device) override;

				private:
					Mesh* m_Mesh;
					Matrix4F m_Model;
					Matrix4F m_View;
					Matrix4F m_Projection;
					Matrix4F m_MVP;
					Program* m_Program;
					ProgramConstantHolder::BufferDataMap m_Buffers;
					ProgramConstantHolder::TextureDataMap m_Textures;
				};

				class ProgramDrawCommand : public DrawCommandBase
				{
				public:
					ProgramDrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program) :
						DrawCommandBase(Allocator, Mesh, Model, View, Projection, MVP, Program, Program->GetBuffers(), Program->GetTextures())
					{
					}
				};

				class PassDrawCommand : public DrawCommandBase
				{
				public:
					PassDrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Pass* Pass) :
						DrawCommandBase(Allocator, Mesh, Model, View, Projection, MVP, Pass->GetProgram()->GetPointer(), Pass->GetBuffers(), Pass->GetTextures()),
						m_RenderState(Pass->GetRenderState())
					{
					}

					void Execute(IDevice* Device) override
					{
						Device->SetState(m_RenderState);

						DrawCommandBase::Execute(Device);
					}

				private:
					IDevice::State m_RenderState;
				};
			}
		}
	}
}

#endif