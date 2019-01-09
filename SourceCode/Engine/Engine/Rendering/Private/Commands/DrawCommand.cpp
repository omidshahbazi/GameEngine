// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\IDevice.h>
#include <Rendering\Mesh.h>
#include <Rendering\ProgramConstantSupplier.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				void DrawCommand::Execute(IDevice * Device)
				{
					if (m_Program != nullptr)
					{
						ProgramConstantSupplier::GetInstance()->SupplyConstants(Device, m_Program);
						m_Program->SetMatrix4("_MVP", m_Transform);
					}

					Device->BindProgram((m_Program == nullptr ? 0 : m_Program->GetHandle()));

					for (uint16 i = 0; i < m_Mesh->GetSubMeshCount(); ++i)
					{
						Mesh::SubMesh &subMesh = m_Mesh->GetSubMeshes()[i];

						Device->BindBuffer(subMesh.GetBuffer().GetHandle());

						Device->Draw(IDevice::DrawModes::Triangles, subMesh.GetIndexCount());
					}
				}
			}
		}
	}
}
