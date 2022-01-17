// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\CommandBuffer.h>
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\RenderTarget.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderSystem\Private\Commands.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\FrameDataChain.h>
#include <RenderSystem\Private\GPUConstantBuffer.h>
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
			m_Name(Name),
			m_Buffer(RenderSystemAllocators::CommandBufferAllocator)
		{
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

		void CommandBuffer::SetRenderTarget(const RenderTarget* RenderTarget)
		{
			m_Buffer.Append(CommandTypes::SetRenderTarget);

			SetRenderTargetCommandData data = {};
			data.RenderTarget = ConstCast(RenderSystem::RenderTarget*, RenderTarget);

			m_Buffer.Append(data);
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
			data.MVP = Projection;
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

		void CommandBuffer::PrepareNativeBuffers(ThreadedDevice* Device, FrameDataChain* FrameDataChain, RenderContext* RenderContext, NativeCommandBufferList& NativeCommandBuffers)
		{
			static const ICommandBuffer::Types TypePerCommand[] = { ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics };

			const WString name = m_Name.ChangeType<char16>();

			ICommandBuffer* copyConstantBuffersCB = nullptr;
			CoreDebugAssert(Categories::RenderSystem, Device->CreateCommandBuffer(ICommandBuffer::Types::Copy, copyConstantBuffersCB).Wait(), "Couldn't create a native command buffer");
			copyConstantBuffersCB->SetName(name.GetValue());
			bool hasAnyCopy = false;

			ICommandBuffer* currentCB = nullptr;

			m_Buffer.ResetRead();
			CommandTypes commandType;
			while (m_Buffer.Read(commandType))
			{
				const ICommandBuffer::Types desiredType = TypePerCommand[(uint8)commandType];

				if (currentCB == nullptr || currentCB->GetType() == desiredType)
				{
					CoreDebugAssert(Categories::RenderSystem, Device->CreateCommandBuffer(desiredType, currentCB).Wait(), "Couldn't create a native command buffer");

					currentCB->SetName(name.GetValue());

					NativeCommandBuffers.Add(currentCB);

					//Set viewport for each new command buffer to the Context
					//Set prev viewport and bounded render-target
				}

				switch (commandType)
				{
				case CommandTypes::GenerateMipMap:
				{
					GenerateMipMapCommandData data = {};
					m_Buffer.Read(data);

					currentCB->GenerateMipMap(data.Texture->GetHandle());
				} break;

				case CommandTypes::SetRenderTarget:
				{
					SetRenderTargetCommandData data = {};
					m_Buffer.Read(data);

					ResourceHandle handle = 0;
					Vector2I size = RenderContext->GetWindow()->GetClientSize();

					if (data.RenderTarget != nullptr)
					{
						handle = data.RenderTarget->GetHandle();
						size = data.RenderTarget->GetTexture(0)->GetDimension();
					}

					currentCB->SetRenderTarget(data.RenderTarget == nullptr ? 0 : data.RenderTarget->GetHandle());

					currentCB->SetViewport(Vector2I::Zero, size);
				} break;

				case CommandTypes::SetViewport:
				{
					SetViewportCommandData data = {};
					m_Buffer.Read(data);

					currentCB->SetViewport(data.Position, data.Size);
				} break;

				case CommandTypes::Clear:
				{
					ClearCommandData data = {};
					m_Buffer.Read(data);

					currentCB->Clear(data.Flags, data.Color);
				} break;

				case CommandTypes::Draw:
				{
					DrawCommandData data = {};
					m_Buffer.Read(data);

					hasAnyCopy = true;

					InsertDrawCommand(FrameDataChain, copyConstantBuffersCB, currentCB, data.Mesh, data.Model, data.View, data.Projection, data.MVP, data.Material);
				} break;

				case CommandTypes::BeginEvent:
				{
					BeginEventCommandData data = {};
					m_Buffer.Read(data);

					currentCB->BeginEvent(data.Label.GetValue());
				} break;

				case CommandTypes::EndEvent:
				{
					EndEventCommandData data = {};
					m_Buffer.Read(data);

					currentCB->EndEvent();
				} break;

				case CommandTypes::SetMarker:
				{
					SetMarkerCommandData data = {};
					m_Buffer.Read(data);

					currentCB->SetMarker(data.Label.GetValue());
				} break;

				default:
					CoreDebugAssert(Categories::RenderSystem, false, "CommandType is not recognized");
				}
			}

			if (hasAnyCopy)
				NativeCommandBuffers.Insert(0, copyConstantBuffersCB);
			else
				CoreDebugAssert(Categories::RenderSystem, Device->DestroyCommandBuffer(&copyConstantBuffersCB, 1).Wait(), "Couldn't destroy a native command buffer");
		}

		void CommandBuffer::InsertDrawCommand(FrameDataChain* FrameDataChain, ICommandBuffer* CopyConstantBuffersCB, ICommandBuffer* GraphicsCB, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
			static BuiltiInProgramConstants::TransformData data;
			data.Model = Model;
			data.View = View;
			data.Projection = Projection;
			data.MVP = MVP;
			BuiltiInProgramConstants::GetInstance()->SetTransfomData(data);

			for (auto& pass : Material->GetPasses())
			{
				const ProgramResource* program = pass.GetProgram();

				GraphicsCB->SetProgram(program->GetPointer()->GetHandle());

				ProgramConstantSupplier::GPUBufferDataBaseMap buffers;
				for (auto& info : pass.GetBuffers())
				{
					auto& constantInfo = info.GetSecond();
					auto& localConstantInfo = buffers[info.GetFirst()];

					localConstantInfo.Handle = constantInfo.Handle;
					localConstantInfo.Value = FrameDataChain->GetFrontConstantBuffers()->Get(constantInfo.Value->GetSize());
					localConstantInfo.Value->Copy(constantInfo.Value);
				}

				ProgramConstantHolder::TextureDataBaseMap textures;
				for (auto& info : pass.GetTextures())
					textures[info.GetFirst()] = info.GetSecond();

				ProgramConstantSupplier::GetInstance()->SupplyConstants(buffers, textures);

				for (auto& info : buffers)
				{
					auto& constant = info.GetSecond();

					constant.Value->UploadToGPU();

					GraphicsCB->SetProgramConstantBuffer(constant.Handle, constant.Value->GetHandle());
				}

				for (auto& info : textures)
				{
					auto& constant = info.GetSecond();

					ResourceHandle texHandle = 0;
					if (constant.Value != nullptr && !constant.Value->IsNull())
					{
						Texture* tex = constant.Value->GetPointer();

						texHandle = tex->GetHandle();
					}

					GraphicsCB->SetProgramTexture(constant.Handle, texHandle);
				}

				for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
				{
					SubMesh& subMesh = Mesh->GetSubMeshes()[i];

					GraphicsCB->SetMesh(subMesh.GetHandle());

					const uint16 idxCount = subMesh.GetIndexCount();
					if (idxCount == 0)
						GraphicsCB->DrawArray(subMesh.GetPolygonType(), subMesh.GetVertexCount());
					else
						GraphicsCB->DrawIndexed(subMesh.GetPolygonType(), idxCount);
				}
			}
		}
	}
}