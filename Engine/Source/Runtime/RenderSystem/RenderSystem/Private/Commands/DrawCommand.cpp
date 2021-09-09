// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\Commands\DrawCommand.h>
#include <RenderSystem\Private\Commands\IntermediateConstantBuffers.h>
#include <RenderSystem\Private\BuiltiInProgramConstants.h>
#include <RenderSystem\Private\Commands\GPUConstantBuffer.h>
#include <RenderDevice\IDevice.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ProgramConstantSupplier.h>

namespace Engine
{
	namespace RenderSystem
	{
		namespace Private
		{
			namespace Commands
			{
				DrawCommand::DrawCommand(AllocatorBase* Allocator, IntermediateConstantBuffers* IntermediateConstantBuffers, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, const ProgramConstantHolder* Constants) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Program(Program),
					m_Buffers(Allocator),
					m_Textures(Allocator)
				{
					for (auto& info : Constants->GetBuffers())
					{
						auto& constantInfo = info.GetSecond();
						auto& localConstantInfo = m_Buffers[info.GetFirst()];

						localConstantInfo.Handle = constantInfo.Handle;
						localConstantInfo.Value = IntermediateConstantBuffers->Get(constantInfo.Value->GetSize());
						localConstantInfo.Value->Copy(constantInfo.Value);
					}

					for (auto& info : Constants->GetTextures())
						m_Textures[info.GetFirst()] = info.GetSecond();
				}

				void DrawCommand::Execute(IDevice* Device)
				{
					static BuiltiInProgramConstants::TransformData data;
					data.Model = m_Model;
					data.View = m_View;
					data.Projection = m_Projection;
					data.MVP = m_MVP;
					BuiltiInProgramConstants::GetInstance()->SetTransfomData(data);

					if (m_Program == nullptr)
						Device->BindProgram(0);
					else
					{
						Device->BindProgram(m_Program->GetHandle());

						ProgramConstantSupplier::GetInstance()->SupplyConstants(m_Buffers, m_Textures);

						for (auto& info : m_Buffers)
						{
							auto& constant = info.GetSecond();

							constant.Value->UploadToGPU();

							Device->SetProgramConstantBuffer(constant.Handle, constant.Value->GetHandle());
						}

						for (auto& info : m_Textures)
						{
							auto& constant = info.GetSecond();

							ResourceHandle texHandle = 0;
							TextureTypes type = TextureTypes::TwoD;
							if (constant.Value != nullptr && !constant.Value->IsNull())
							{
								Texture* tex = constant.Value->GetPointer();

								texHandle = tex->GetHandle();
								type = tex->GetType();
							}

							Device->SetProgramTexture(constant.Handle, type, texHandle);
						}
					}

					for (uint16 i = 0; i < m_Mesh->GetSubMeshCount(); ++i)
					{
						SubMesh& subMesh = m_Mesh->GetSubMeshes()[i];

						Device->BindMesh(subMesh.GetHandle());

						const uint16 idxCount = subMesh.GetIndexCount();
						if (idxCount == 0)
							Device->DrawArray(subMesh.GetPolygonType(), subMesh.GetVertexCount());
						else
							Device->DrawIndexed(subMesh.GetPolygonType(), idxCount);
					}
				}
			}
		}
	}
}