// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\IDevice.h>
#include <Rendering\Mesh.h>
#include <Rendering\Pass.h>
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Private\BuiltiInProgramConstants.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				DrawCommand::DrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, const ProgramConstantHolder* Constants) :
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
						m_Buffers[info.GetFirst()] = info.GetSecond();

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

						ProgramConstantSupplier::GetInstance()->SupplyConstants(Device, m_Buffers, m_Textures);

						for (auto& info : m_Buffers)
						{
							auto& constant = info.GetSecond();

							if (constant.Value == nullptr)
								continue;

							Device->SetProgramConstantBuffer(constant.Handle, constant.Value->GetHandle());
						}

						for (auto& info : m_Textures)
						{
							auto& constant = info.GetSecond();

							Texture::Handle texHandle = 0;
							Texture::Types type = Texture::Types::TwoD;
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