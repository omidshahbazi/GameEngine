// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
				DrawCommand::DrawCommand(Mesh * Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Program * Program) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Program(Program),
					m_Pass(nullptr)
				{
				}

				DrawCommand::DrawCommand(Mesh * Mesh, const Matrix4F & Model, const Matrix4F &View, const Matrix4F &Projection, const Matrix4F &MVP, Pass * Pass) :
					m_Mesh(Mesh),
					m_Model(Model),
					m_View(View),
					m_Projection(Projection),
					m_MVP(MVP),
					m_Program(nullptr),
					m_Pass(Pass)
				{
					m_Program = Pass->GetProgram()->GetData();
				}

				void DrawCommand::Execute(IDevice * Device)
				{
					if (m_Pass != nullptr)
						Device->SetState(m_Pass->GetRenderState());

					if (m_Program != nullptr)
					{
						Device->BindProgram(m_Program->GetHandle());

						if (m_Pass != nullptr)
							m_Program->ApplyConstantValue(m_Pass->GetConstants());

						ProgramConstantSupplier::GetInstance()->SupplyConstants(Device, m_Program);
						m_Program->SetMatrix4("_Model", m_Model);
						m_Program->SetMatrix4("_View", m_View);
						m_Program->SetMatrix4("_Projection", m_Projection);
						m_Program->SetMatrix4("_MVP", m_MVP);
					}
					else
						Device->BindProgram(0);

					for (uint16 i = 0; i < m_Mesh->GetSubMeshCount(); ++i)
					{
						Mesh::SubMesh &subMesh = m_Mesh->GetSubMeshes()[i];

						Device->BindMesh(subMesh.GetBuffer().GetHandle());

						Device->Draw(IDevice::DrawModes::Triangles, subMesh.GetIndexCount());
					}
				}
			}
		}
	}
}
