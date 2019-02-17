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
				DrawCommand::DrawCommand(MeshHandle * Mesh, const Matrix4F & Transform, ProgramHandle * Program) :
					m_Mesh(Mesh),
					m_Transform(Transform),
					m_Program(Program),
					m_Pass(nullptr)
				{
				}

				DrawCommand::DrawCommand(MeshHandle * Mesh, const Matrix4F & Transform, Pass * Pass) :
					m_Mesh(Mesh),
					m_Transform(Transform),
					m_Program(nullptr),
					m_Pass(Pass)
				{
					m_Program = Pass->GetProgram();
				}

				void DrawCommand::Execute(IDevice * Device)
				{
					if (m_Pass != nullptr)
						Device->SetState(m_Pass->GetRenderState());

					if (m_Program != nullptr)
					{
						Program *program = **m_Program;

						Device->BindProgram(program->GetHandle());

						if (m_Pass != nullptr)
							program->ApplyConstantValue(m_Pass->GetConstants());

						ProgramConstantSupplier::GetInstance()->SupplyConstants(Device, program);
						program->SetMatrix4("_MVP", m_Transform);
					}
					else
						Device->BindProgram(0);

					Mesh *mesh = **m_Mesh;

					for (uint16 i = 0; i < mesh->GetSubMeshCount(); ++i)
					{
						Mesh::SubMesh &subMesh = mesh->GetSubMeshes()[i];

						Device->BindMesh(subMesh.GetBuffer().GetHandle());

						Device->Draw(IDevice::DrawModes::Triangles, subMesh.GetIndexCount());
					}
				}
			}
		}
	}
}
