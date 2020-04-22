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
				DrawCommand::DrawCommand(Mesh * Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Shader* Shader) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Shader(Shader),
					m_Pass(nullptr)
				{
				}

				DrawCommand::DrawCommand(Mesh * Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Pass * Pass) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Shader(nullptr),
					m_Pass(Pass)
				{
					m_Shader = Pass->GetShader()->GetData();
				}

				void DrawCommand::Execute(IDevice * Device)
				{
					if (m_Pass != nullptr)
						Device->SetState(m_Pass->GetRenderState());

					if (m_Shader != nullptr)
					{
						Device->BindShader(m_Shader->GetHandle());

						if (m_Pass != nullptr)
							m_Shader->ApplyConstantValue(m_Pass->GetConstants());

						ShaderConstantSupplier::GetInstance()->SupplyConstants(Device, m_Shader);
						m_Shader->SetMatrix4("_Model", m_Model);
						m_Shader->SetMatrix4("_View", m_View);
						m_Shader->SetMatrix4("_Projection", m_Projection);
						m_Shader->SetMatrix4("_MVP", m_MVP);
					}
					else
						Device->BindShader(0);

					for (uint16 i = 0; i < m_Mesh->GetSubMeshCount(); ++i)
					{
						SubMesh &subMesh = m_Mesh->GetSubMeshes()[i];

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
