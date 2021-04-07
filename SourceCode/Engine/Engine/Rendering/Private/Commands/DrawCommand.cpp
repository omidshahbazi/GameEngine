// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\IDevice.h>
#include <Rendering\Mesh.h>
#include <Rendering\Pass.h>
#include <Rendering\ProgramConstantSupplier.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				DrawCommand::DrawCommand(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Program(Program),
					m_CreatedByPass(false)
				{
				}

				DrawCommand::DrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Pass* Pass) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Program(Pass->GetProgram()->GetPointer()),
					m_CreatedByPass(true),
					m_BufferInfo(Allocator, Pass->GetBuffers()),
					m_TextureInfo(Allocator, Pass->GetTextures()),
					m_RenderState(Pass->GetRenderState())
				{
				}

				void DrawCommand::Execute(IDevice* Device)
				{
					static Program::ConstantHash ConstantHash_MODEL = Program::GetHash("_Model");
					static Program::ConstantHash ConstantHash_VIEW = Program::GetHash("_View");
					static Program::ConstantHash ConstantHash_PROJECTION = Program::GetHash("_Projection");
					static Program::ConstantHash ConstantHash_MVP = Program::GetHash("_MVP");

					if (m_CreatedByPass)
						Device->SetState(m_RenderState);

					if (m_Program != nullptr)
					{
						Device->BindProgram(m_Program->GetHandle());

						ProgramConstantSupplier::GetInstance()->SupplyConstants(m_Program);

						if (m_CreatedByPass)
							m_Program->SetConstantsValue(m_BufferInfo, m_TextureInfo);

						//m_Program->SetMatrix4(ConstantHash_MODEL, m_Model);
						//m_Program->SetMatrix4(ConstantHash_VIEW, m_View);
						//m_Program->SetMatrix4(ConstantHash_PROJECTION, m_Projection);
						//m_Program->SetMatrix4(ConstantHash_MVP, m_MVP);

						m_Program->ApplyConstantsValue(Device);
					}
					else
						Device->BindProgram(0);

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