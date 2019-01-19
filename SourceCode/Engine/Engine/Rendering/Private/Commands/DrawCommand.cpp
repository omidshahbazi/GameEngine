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
				DrawCommand::DrawCommand(Mesh * Mesh, const Matrix4F & Transform, Program * Program) :
					m_Mesh(Mesh),
					m_Transform(Transform),
					m_Program(Program),
					m_Pass(nullptr)
				{
				}

				DrawCommand::DrawCommand(Mesh * Mesh, const Matrix4F & Transform, Pass * Pass) :
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
					{
						Device->SetFaceOrder(m_Pass->GetFaceOrder());
						Device->SetCullMode(m_Pass->GetCullMode());
						Device->SetDepthTestFunction(m_Pass->GetDepthTestFunction());

						IDevice::TestFunctions stencilTestFunction;
						int32 stencilTestFunctionReference;
						uint32 stencilTestFunctionMask;
						m_Pass->GetStencilTestFunction(stencilTestFunction, stencilTestFunctionReference, stencilTestFunctionMask);
						Device->SetStencilTestFunction(stencilTestFunction, stencilTestFunctionReference, stencilTestFunctionMask);

						Device->SetStencilMask(m_Pass->GetStencilMask());

						IDevice::StencilOperations stencilOperationStencilFailed;
						IDevice::StencilOperations stencilOperationDepthFailed;
						IDevice::StencilOperations stencilOperationDepthPassed;
						m_Pass->GetStencilOperation(stencilOperationStencilFailed, stencilOperationDepthFailed, stencilOperationDepthPassed);
						Device->SetStencilOperation(stencilOperationStencilFailed, stencilOperationDepthFailed, stencilOperationDepthPassed);

						IDevice::BlendFunctions blendFunctionSourceFactor;
						IDevice::BlendFunctions blendFunctionDestinationFactor;
						m_Pass->GetBlendFunction(blendFunctionSourceFactor, blendFunctionDestinationFactor);
						Device->SetBlendFunction(blendFunctionSourceFactor, blendFunctionDestinationFactor);

						IDevice::CullModes polygonModeCullMode;
						IDevice::PolygonModes polygonMode;
						m_Pass->GetPolygonMode(polygonModeCullMode, polygonMode);
						Device->SetPolygonMode(polygonModeCullMode, polygonMode);
					}

					if (m_Program != nullptr)
					{
						if (m_Pass != nullptr)
							m_Program->ApplyConstantValue(m_Pass->GetConstants());

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
