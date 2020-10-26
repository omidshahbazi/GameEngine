// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\IDevice.h>
#include <Rendering\Mesh.h>
#include <Rendering\Pass.h>
#include <Rendering\ShaderConstantSupplier.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				DrawCommand::DrawCommand(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Shader* Shader) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Shader(Shader),
					m_CreatedByPass(false)
				{
				}

				DrawCommand::DrawCommand(AllocatorBase* Allocator, Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Pass* Pass) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Shader(Pass->GetShader()->GetPointer()),
					m_CreatedByPass(true),
					m_Constants(Allocator, Pass->GetConstants()),
					m_RenderState(Pass->GetRenderState())
				{
				}

				void DrawCommand::Execute(IDevice* Device)
				{
					static Shader::ConstantHash ConstantHash_MODEL = Shader::GetHash("_Model");
					static Shader::ConstantHash ConstantHash_VIEW = Shader::GetHash("_View");
					static Shader::ConstantHash ConstantHash_PROJECTION = Shader::GetHash("_Projection");
					static Shader::ConstantHash ConstantHash_MVP = Shader::GetHash("_MVP");

					if (m_CreatedByPass)
						Device->SetState(m_RenderState);

					if (m_Shader != nullptr)
					{
						Device->BindShader(m_Shader->GetHandle());

						ShaderConstantSupplier::GetInstance()->SupplyConstants(m_Shader);

						if (m_CreatedByPass)
							m_Shader->SetConstantsValue(m_Constants);

						m_Shader->SetMatrix4(ConstantHash_MODEL, m_Model);
						m_Shader->SetMatrix4(ConstantHash_VIEW, m_View);
						m_Shader->SetMatrix4(ConstantHash_PROJECTION, m_Projection);
						m_Shader->SetMatrix4(ConstantHash_MVP, m_MVP);

						m_Shader->ApplyConstantsValue(Device);
					}
					else
						Device->BindShader(0);

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