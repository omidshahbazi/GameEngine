// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\CommandBuffer.h>
#include <RenderSystem\Private\Commands\Commands.h>
#include <RenderSystem\RenderTarget.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderSystem\Private\BuiltiInProgramConstants.h>
#include <RenderDevice\ICommandBuffer.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	namespace RenderSystem
	{
		using namespace Private::Commands;
		using namespace RenderCommon::Private;

		CommandBuffer::CommandBuffer(ICommandBuffer* NativeBuffer) :
			m_Buffer(RenderSystemAllocators::CommandBufferAllocator),
			m_NativeBuffer(NativeBuffer)
		{
		}

		void CommandBuffer::SetViewport(const Vector2I& Position, const Vector2I& Size)
		{
			m_Buffer.Append(CommandTypes::SetViewport);

			SetViewportCommandData data = {};
			data.Position = Position;
			data.Size = Size;

			m_Buffer.Append(data);
		}

		void CommandBuffer::SetRenderTarget(const RenderTarget* RenderTarget)
		{
			m_Buffer.Append(CommandTypes::SetRenderTarget);

			SetRenderTargetCommandData data = {};
			data.RenderTarget = ConstCast(RenderSystem::RenderTarget*, RenderTarget);

			m_Buffer.Append(data);
		}

		void CommandBuffer::Clear(ClearFlags Flags, const ColorUI8& Color)
		{
			m_Buffer.Append(CommandTypes::Clear);

			ClearCommandData data = {};
			data.Flags = Flags;
			data.Color = Color;

			m_Buffer.Append(data);
		}

		void CommandBuffer::DrawMesh(const Mesh* Mesh, const Matrix4F& Transform, const Material* Material)
		{
			m_Buffer.Append(CommandTypes::Draw);

			DrawCommandData data = {};
			data.Mesh = ConstCast(RenderSystem::Mesh*, Mesh);
			data.Model = Matrix4F::Identity;
			data.View = Matrix4F::Identity;
			data.Projection = Matrix4F::Identity;
			data.MVP = Transform;
			data.Material = ConstCast(RenderSystem::Material*, Material);

			m_Buffer.Append(data);
		}

		void CommandBuffer::DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material)
		{
			m_Buffer.Append(CommandTypes::Draw);

			DrawCommandData data = {};
			data.Mesh = ConstCast(RenderSystem::Mesh*, Mesh);
			data.Model = Model;
			data.View = View;
			data.Projection = Projection;

			data.MVP = Projection;
			data.MVP *= View;
			data.MVP *= Model;

			data.Material = ConstCast(RenderSystem::Material*, Material);

			m_Buffer.Append(data);
		}

		void CommandBuffer::DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
			m_Buffer.Append(CommandTypes::Draw);

			DrawCommandData data = {};
			data.Mesh = ConstCast(RenderSystem::Mesh*, Mesh);
			data.Model = Model;
			data.View = View;
			data.Projection = Projection;

			data.MVP = Projection;
			data.MVP *= View;
			data.MVP *= Model;

			data.Material = ConstCast(RenderSystem::Material*, Material);

			m_Buffer.Append(data);
		}

		void CommandBuffer::BeginEvent(const String& Label)
		{
			BeginEvent(Label.ChangeType<char16>());
		}

		void CommandBuffer::BeginEvent(const WString& Label)
		{
			m_Buffer.Append(CommandTypes::BeginEvent);

			BeginEventCommandData data = {};
			data.Label = Label;

			m_Buffer.Append(data);
		}

		void CommandBuffer::EndEvent(void)
		{
			m_Buffer.Append(CommandTypes::EndEvent);

			EndEventCommandData data = {};

			m_Buffer.Append(data);
		}

		void CommandBuffer::SetMarker(const String& Label)
		{
			SetMarker(Label.ChangeType<char16>());
		}

		void CommandBuffer::SetMarker(const WString& Label)
		{
			m_Buffer.Append(CommandTypes::SetMarker);

			SetMarkerCommandData data = {};
			data.Label = Label;

			m_Buffer.Append(data);
		}

		ICommandBuffer* CommandBuffer::PrepareNativeBuffer(void)
		{
			m_NativeBuffer->Clear();
			m_Buffer.ResetRead();

			CommandTypes commandType;
			while (m_Buffer.Read(commandType))
			{
				switch (commandType)
				{
				case CommandTypes::SetViewport:
				{
					SetViewportCommandData data = {};
					m_Buffer.Read(data);

					m_NativeBuffer->SetViewport(data.Position, data.Size);
				} break;

				case CommandTypes::SetRenderTarget:
				{
					SetRenderTargetCommandData data = {};
					m_Buffer.Read(data);

					m_NativeBuffer->SetRenderTarget(data.RenderTarget == nullptr ? 0 : data.RenderTarget->GetHandle());
				} break;

				case CommandTypes::Clear:
				{
					ClearCommandData data = {};
					m_Buffer.Read(data);

					m_NativeBuffer->Clear(data.Flags, data.Color);
				} break;

				case CommandTypes::Draw:
				{
					DrawCommandData data = {};
					m_Buffer.Read(data);

					InsertDrawCommand(data.Mesh, data.Model, data.View, data.Projection, data.MVP, data.Material);
				} break;

				case CommandTypes::BeginEvent:
				{
					BeginEventCommandData data = {};
					m_Buffer.Read(data);

					m_NativeBuffer->BeginEvent(data.Label.GetValue());
				} break;

				case CommandTypes::EndEvent:
				{
					EndEventCommandData data = {};
					m_Buffer.Read(data);

					m_NativeBuffer->EndEvent();
				} break;

				case CommandTypes::SetMarker:
				{
					SetMarkerCommandData data = {};
					m_Buffer.Read(data);

					m_NativeBuffer->SetMarker(data.Label.GetValue());
				} break;

				default:
					CoreDebugAssert(Categories::RenderSystem, false, "CommandType is not recognized");
				}
			}

			return m_NativeBuffer;
		}

		void CommandBuffer::InsertDrawCommand(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
			static BuiltiInProgramConstants::TransformData data;
			data.Model = Model;
			data.View = View;
			data.Projection = Projection;
			data.MVP = MVP;
			BuiltiInProgramConstants::GetInstance()->SetTransfomData(data);

			for (auto& pass : Material->GetPasses())
			{
				ProgramResource* program = pass.GetProgram();

				if (program->IsNull())
					return;

				m_NativeBuffer->SetProgram(program->GetPointer()->GetHandle());

				ProgramConstantSupplier::GetInstance()->SupplyConstants(m_Buffers, m_Textures);

				for (auto& info : m_Buffers)
				{
					auto& constant = info.GetSecond();

					constant.Value->UploadToGPU();

					m_NativeBuffer->SetProgramConstantBuffer(constant.Handle, constant.Value->GetHandle());
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

					m_NativeBuffer->SetProgramTexture(constant.Handle, type, texHandle);
				}

				for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
				{
					SubMesh& subMesh = Mesh->GetSubMeshes()[i];

					m_NativeBuffer->SetMesh(subMesh.GetHandle());

					const uint16 idxCount = subMesh.GetIndexCount();
					if (idxCount == 0)
						m_NativeBuffer->DrawArray(subMesh.GetPolygonType(), subMesh.GetVertexCount());
					else
						m_NativeBuffer->DrawIndexed(subMesh.GetPolygonType(), idxCount);
				}
			}
		}
	}
}