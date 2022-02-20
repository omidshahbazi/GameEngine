// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\CommandBuffer.h>
#include <RenderSystem\CommandBufferFence.h>
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\RenderTarget.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderSystem\Private\Commands.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\FrameConstantBuffers.h>
#include <RenderSystem\Private\GPUConstantBuffer.h>
#include <RenderSystem\Private\InternalRenderTargets.h>
#include <RenderSystem\Private\BuiltiInProgramConstants.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderDevice\ICommandBuffer.h>
#include <WindowUtility\Window.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	namespace RenderSystem
	{
		using namespace Private;
		using namespace RenderCommon::Private;
		using namespace WindowUtility;

		CommandBuffer::CommandBuffer(const String& Name) :
			m_Buffer(RenderSystemAllocators::CommandBufferAllocator)
		{
			SetName(Name);
		}

		bool CommandBuffer::CopyTexture(const Texture* Source, const Texture* Destination)
		{
			if (Source == nullptr)
				return false;

			return CopyTexture(Source, Vector2I::Zero, Destination, Vector2I::Zero, Source->GetDimension());
		}

		bool CommandBuffer::CopyTexture(const Texture* Source, const Texture* Destination, const Vector2I& Position, const Vector2I& Size)
		{
			return CopyTexture(Source, Position, Destination, Position, Size);
		}

		bool CommandBuffer::CopyTexture(const Texture* Source, const Vector2I& SourcePosition, const Texture* Destination, const Vector2I& DestinationPosition, const Vector2I& Size)
		{
			if (Source == nullptr)
				return false;

			if (SourcePosition < Vector2I::Zero)
				return false;

			if (Destination == nullptr)
				return false;

			if (DestinationPosition < Vector2I::Zero)
				return false;

			if (Size <= Vector2I::Zero)
				return false;

			if (Source->GetFormat() != Destination->GetFormat())
				return false;

			m_Buffer.Append(CommandTypes::CopyTexture);

			CopyTextureCommandData data = {};
			data.Source = ConstCast(Texture*, Source);
			data.SourcePosition = SourcePosition;
			data.Destination = ConstCast(Texture*, Destination);
			data.DestinationPosition = DestinationPosition;
			data.Size = Size;

			m_Buffer.Append(data);

			return true;
		}

		bool CommandBuffer::GenerateMipMap(const Texture* Texture)
		{
			if (Texture == nullptr)
				return false;

			m_Buffer.Append(CommandTypes::GenerateMipMap);

			GenerateMipMapCommandData data = {};
			data.Texture = ConstCast(RenderSystem::Texture*, Texture);

			m_Buffer.Append(data);

			return true;
		}

		bool CommandBuffer::SetRenderTarget(RenderTargets RenderTarget)
		{
			m_Buffer.Append(CommandTypes::SetRenderTarget);

			SetRenderTargetCommandData data = {};
			data.RenderTarget = ConstCast(RenderSystem::RenderTarget*, InternalRenderTargets::GetInstance()->Get(RenderTarget));

			m_Buffer.Append(data);

			return true;
		}

		bool CommandBuffer::SetRenderTarget(const RenderTarget* RenderTarget)
		{
			if (RenderTarget == nullptr)
				return false;

			m_Buffer.Append(CommandTypes::SetRenderTarget);

			SetRenderTargetCommandData data = {};
			data.RenderTarget = ConstCast(RenderSystem::RenderTarget*, RenderTarget);

			m_Buffer.Append(data);

			return true;
		}

		bool CommandBuffer::SetViewport(const Vector2I& Position, const Vector2I& Size)
		{
			if (Position.X < 0 || Position.Y < 0)
				return false;

			if (Size.X < 0 || Size.Y < 0)
				return false;

			m_Buffer.Append(CommandTypes::SetViewport);

			SetViewportCommandData data = {};
			data.Position = Position;
			data.Size = Size;

			m_Buffer.Append(data);

			return true;
		}

		void CommandBuffer::Clear(ClearFlags Flags, const ColorUI8& Color)
		{
			m_Buffer.Append(CommandTypes::Clear);

			ClearCommandData data = {};
			data.Flags = Flags;
			data.Color = Color;

			m_Buffer.Append(data);
		}

		bool CommandBuffer::DrawMesh(const Mesh* Mesh, const Matrix4F& Transform, const Material* Material)
		{
			return DrawMesh(Mesh, Matrix4F::Identity, Matrix4F::Identity, Matrix4F::Identity, Transform, Material);
		}

		bool CommandBuffer::DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material)
		{
			Matrix4F mvp = Projection;
			mvp *= View;
			mvp *= Model;

			return DrawMesh(Mesh, Model, View, Projection, mvp, Material);
		}

		bool CommandBuffer::DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
			if (Mesh == nullptr)
				return false;

			if (Material == nullptr)
				return false;

			m_Buffer.Append(CommandTypes::Draw);

			DrawCommandData data = {};
			data.Mesh = ConstCast(RenderSystem::Mesh*, Mesh);
			data.Model = Model;
			data.View = View;
			data.Projection = Projection;
			data.MVP = MVP;
			data.Material = ConstCast(RenderSystem::Material*, Material);

			m_Buffer.Append(data);

			return true;
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

		bool CommandBuffer::WaitForFences(CommandBufferFence* const* Fences, uint16 Count)
		{
			if (Count == 0)
				return false;

			for (uint8 i = 0; i < Count; ++i)
				if (Fences[i] == nullptr)
					return false;

			m_Buffer.Append(CommandTypes::WaitForFences);

			WaitForFencesCommandData data = {};
			data.Fences = Fences;
			data.Count = Count;

			m_Buffer.Append(data);
		}

		bool CommandBuffer::SignalFences(CommandBufferFence* const* Fences, uint16 Count)
		{
			if (Count == 0)
				return false;

			for (uint8 i = 0; i < Count; ++i)
				if (Fences[i] == nullptr)
					return false;

			m_Buffer.Append(CommandTypes::SignalFences);

			SignalFencesCommandData data = {};
			data.Fences = Fences;
			data.Count = Count;

			m_Buffer.Append(data);
		}

		bool CommandBuffer::PrepareNativeBuffers(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, Texture* DefaultTexture, const RenderContext* RenderContext)
		{
			CoreDebugAssert(Categories::RenderSystem, CommandBuffer != nullptr, "CommandBuffer cannot be null");
			CoreDebugAssert(Categories::RenderSystem, ConstantBuffers != nullptr, "ConstantBuffers cannot be null");

			if (m_Buffer.GetSize() == 0)
				return false;

#define SET_RENDER_TARGET(RenderTarget) \
			m_LastRenderTarget = RenderTarget; \
			CommandBuffer->SetRenderTarget(m_LastRenderTarget);

#define SET_VIEWPORT(Position, Size) \
			m_LastViewportPosition = Position; \
			m_LastViewportSize = Size; \
			CommandBuffer->SetViewport(m_LastViewportPosition, m_LastViewportSize);

			ResourceHandle m_LastRenderTarget = 0;
			Vector2I m_LastViewportPosition;
			Vector2I m_LastViewportSize;

			if (RenderContext != nullptr)
				m_LastViewportSize = RenderContext->GetWindow()->GetClientSize();

			CommandBuffer->SetName(m_Name.GetValue());
			CommandBuffer->BeginEvent(m_Name.GetValue());

			SET_RENDER_TARGET(m_LastRenderTarget);
			SET_VIEWPORT(m_LastViewportPosition, m_LastViewportSize);

			IFence* fences[1 << (sizeof(uint8) * 8) + 1];

			m_Buffer.ResetRead();
			CommandTypes commandType;
			while (m_Buffer.Read(commandType))
			{
				switch (commandType)
				{
				case CommandTypes::CopyTexture:
				{
					CopyTextureCommandData data = {};
					m_Buffer.Read(data);

					CommandBuffer->CopyTexture(data.Source->GetHandle(), data.SourcePosition, data.Destination->GetHandle(), data.DestinationPosition, data.Size);
				} break;

				case CommandTypes::GenerateMipMap:
				{
					GenerateMipMapCommandData data = {};
					m_Buffer.Read(data);

					CommandBuffer->GenerateMipMap(data.Texture->GetHandle());
				} break;

				case CommandTypes::SetRenderTarget:
				{
					SetRenderTargetCommandData data = {};
					m_Buffer.Read(data);

					ResourceHandle handle = 0;
					Vector2I size;

					if (RenderContext != nullptr)
						size = RenderContext->GetWindow()->GetClientSize();

					if (data.RenderTarget != nullptr)
					{
						handle = data.RenderTarget->GetHandle();
						size = data.RenderTarget->GetTexture(0)->GetDimension();
					}

					SET_RENDER_TARGET(handle);
					SET_VIEWPORT(Vector2I::Zero, size);
				} break;

				case CommandTypes::SetViewport:
				{
					SetViewportCommandData data = {};
					m_Buffer.Read(data);

					SET_VIEWPORT(data.Position, data.Size);
				} break;

				case CommandTypes::Clear:
				{
					ClearCommandData data = {};
					m_Buffer.Read(data);

					CommandBuffer->Clear(data.Flags, data.Color);
				} break;

				case CommandTypes::Draw:
				{
					DrawCommandData data = {};
					m_Buffer.Read(data);

					InsertDrawCommand(CommandBuffer, ConstantBuffers, DefaultTexture, data.Mesh, data.Model, data.View, data.Projection, data.MVP, data.Material);
				} break;

				case CommandTypes::BeginEvent:
				{
					BeginEventCommandData data = {};
					m_Buffer.Read(data);

					CommandBuffer->BeginEvent(data.Label.GetValue());
				} break;

				case CommandTypes::EndEvent:
				{
					EndEventCommandData data = {};
					m_Buffer.Read(data);

					CommandBuffer->EndEvent();
				} break;

				case CommandTypes::SetMarker:
				{
					SetMarkerCommandData data = {};
					m_Buffer.Read(data);

					CommandBuffer->SetMarker(data.Label.GetValue());
				} break;

				case CommandTypes::WaitForFences:
				{
					WaitForFencesCommandData data = {};
					m_Buffer.Read(data);

					for (uint8 i = 0; i < data.Count; ++i)
						fences[i] = data.Fences[i]->GetFence();

					CommandBuffer->WaitForFences(fences, data.Count);
				} break;

				case CommandTypes::SignalFences:
				{
					SignalFencesCommandData data = {};
					m_Buffer.Read(data);

					for (uint8 i = 0; i < data.Count; ++i)
						fences[i] = data.Fences[i]->GetFence();

					CommandBuffer->SignalFences(fences, data.Count);
				} break;

				default:
					CoreDebugAssert(Categories::RenderSystem, false, "CommandType is not recognized");
				}
			}

			CommandBuffer->EndEvent();

			return true;

#undef SET_VIEWPORT
		}

		void CommandBuffer::InsertDrawCommand(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, Texture* DefaultTexture, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
			static BuiltiInProgramConstants::TransformData data;
			data.Model = Model;
			data.View = View;
			data.Projection = Projection;
			data.MVP = MVP;
			BuiltiInProgramConstants::GetInstance()->SetTransfomData(data);

			auto& material = *Material;

			CommandBuffer->SetState(material.GetRenderState());

			CommandBuffer->SetProgram(material.GetProgram()->GetPointer()->GetHandle());

			ProgramConstantSupplier::GPUBufferDataMap buffers;
			for (auto& info : material.GetBuffers())
			{
				auto& constantInfo = info.GetSecond();
				auto& localConstantInfo = buffers[info.GetFirst()];

				localConstantInfo.Handle = constantInfo.Handle;
				localConstantInfo.Value = ConstantBuffers->Get(constantInfo.Value->GetSize());
				localConstantInfo.Value->Copy(constantInfo.Value);
			}

			ProgramConstantSupplier::TextureDataMap textures;
			for (auto& info : material.GetTextures())
				textures[info.GetFirst()] = info.GetSecond();

			ProgramConstantSupplier::GetInstance()->SupplyConstants(buffers, textures);

			for (auto& info : buffers)
			{
				auto& constant = info.GetSecond();

				CommandBuffer->SetProgramConstantBuffer(constant.Handle, constant.Value->GetHandle());
			}

			for (auto& info : textures)
			{
				auto& constant = info.GetSecond();

				ResourceHandle texHandle = 0;
				if (constant.Value != nullptr && !constant.Value->IsNull())
					texHandle = constant.Value->GetPointer()->GetHandle();
				else
					texHandle = DefaultTexture->GetHandle();

				CommandBuffer->SetProgramTexture(constant.Handle, texHandle);
			}

			for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
			{
				SubMesh& subMesh = Mesh->GetSubMeshes()[i];

				CommandBuffer->SetMesh(subMesh.GetHandle());

				const uint16 idxCount = subMesh.GetIndexCount();
				if (idxCount == 0)
					CommandBuffer->DrawArray(subMesh.GetPolygonType(), subMesh.GetVertexCount());
				else
					CommandBuffer->DrawIndexed(subMesh.GetPolygonType(), idxCount);
			}
		}
	}
}