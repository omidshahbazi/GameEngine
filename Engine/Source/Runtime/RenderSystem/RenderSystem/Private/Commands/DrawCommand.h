// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DRAW_COMMAND_H
#define DRAW_COMMAND_H

#include <RenderSystem\Private\Commands\CommandBase.h>
#include <MathContainers\MathContainers.h>
#include <RenderSystem\Pass.h>

namespace Engine
{
	using namespace MathContainers;

	namespace RenderSystem
	{
		class Mesh;

		namespace Private
		{
			class GPUConstantBuffer;
			namespace Commands
			{
				class IntermediateConstantBuffers;

				class DrawCommand : public CommandBase
				{
				private:
					typedef ProgramConstantHolder::ConstantDataBase<GPUConstantBuffer*> GPUBufferConstantDataBase;
					typedef Map<ProgramConstantHolder::ConstantHash, GPUBufferConstantDataBase> GPUBufferDataBaseMap;

				public:
					DrawCommand(AllocatorBase* Allocator, IntermediateConstantBuffers* IntermediateConstantBuffers, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, const ProgramConstantHolder* ConstantsHolder);

					virtual void Execute(IDevice* Device) override;

				private:
					Mesh* m_Mesh;
					Matrix4F m_Model;
					Matrix4F m_View;
					Matrix4F m_Projection;
					Matrix4F m_MVP;
					Program* m_Program;
					GPUBufferDataBaseMap m_Buffers;
					ProgramConstantHolder::TextureDataBaseMap m_Textures;
				};

				class DrawCommandEx : public DrawCommand
				{
				public:
					DrawCommandEx(AllocatorBase* Allocator, IntermediateConstantBuffers* IntermediateConstantBuffers, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, const ProgramConstantHolder* ConstantsHolder, const RenderState& State) :
						DrawCommand(Allocator, IntermediateConstantBuffers, Mesh, Model, View, Projection, MVP, Program, ConstantsHolder),
						m_RenderState(State)
					{
					}

					void Execute(IDevice* Device) override
					{
						Device->SetState(m_RenderState);

						DrawCommand::Execute(Device);
					}

				private:
					RenderState m_RenderState;
				};
			}
		}
	}
}

#endif