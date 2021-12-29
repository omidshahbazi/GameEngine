// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\CommandBuffer.h>
#include <RenderSystem\RenderTarget.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderSystem\Private\Commands\Commands.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\GPUConstantBuffer.h>
#include <RenderSystem\Private\BuiltiInProgramConstants.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderDevice\ICommandBuffer.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	namespace RenderSystem
	{
		using namespace Private::Commands;
		using namespace RenderCommon::Private;

		CommandBuffer::CommandBuffer(ThreadedDevice* Device, const String& Name) :
			m_Device(Device),
			m_Name(Name),
			m_Buffer(RenderSystemAllocators::CommandBufferAllocator)
		{
		}

		CommandBuffer::~CommandBuffer(void)
		{
			for (auto cb : m_NativeCommandBufferList)
				m_Device->DestroyCommandBuffer(cb);
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
			data.MVP *= View;
			data.MVP *= Model;

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

		void CommandBuffer::PrepareNativeBuffer(NativeCommandBufferList& NativeCommandBuffers)
		{
			static const ICommandBuffer::Types TypePerCommand[] = { ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics, ICommandBuffer::Types::Graphics };

			auto nativeCommandBuffers(m_NativeCommandBufferList);

			ICommandBuffer* copyConstantBuffersCB = FindOrCreateCommandBuffer(nativeCommandBuffers, ICommandBuffer::Types::Copy);
			copyConstantBuffersCB->BeginEvent((m_Name + ".CopyConstantBuffers").ChangeType<char16>().GetValue());
			bool hasAnyCBC = false;

			ICommandBuffer* currentCB = nullptr;

			m_Buffer.ResetRead();
			CommandTypes commandType;
			while (m_Buffer.Read(commandType))
			{
				const ICommandBuffer::Types desiredType = TypePerCommand[(uint8)commandType];

				if (currentCB == nullptr || currentCB->GetType() == desiredType)
				{
					currentCB = FindOrCreateCommandBuffer(nativeCommandBuffers, desiredType);

					NativeCommandBuffers.Add(currentCB);

					currentCB->BeginEvent(m_Name.ChangeType<char16>().GetValue());
				}

				switch (commandType)
				{
				case CommandTypes::SetViewport:
				{
					SetViewportCommandData data = {};
					m_Buffer.Read(data);

					currentCB->SetViewport(data.Position, data.Size);
				} break;

				case CommandTypes::SetRenderTarget:
				{
					SetRenderTargetCommandData data = {};
					m_Buffer.Read(data);

					currentCB->SetRenderTarget(data.RenderTarget == nullptr ? 0 : data.RenderTarget->GetHandle());
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

					hasAnyCBC = true;

					InsertDrawCommand(copyConstantBuffersCB, currentCB, data.Mesh, data.Model, data.View, data.Projection, data.MVP, data.Material);
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

			if (hasAnyCBC)
				NativeCommandBuffers.Insert(0, copyConstantBuffersCB);

			for (auto ncb : NativeCommandBuffers)
				ncb->EndEvent();

			m_NativeCommandBufferList.Clear();
			m_NativeCommandBufferList.AddRange(nativeCommandBuffers);
			m_NativeCommandBufferList.AddRange(NativeCommandBuffers);
		}

		void CommandBuffer::InsertDrawCommand(ICommandBuffer* CopyConstantBuffersCB, ICommandBuffer* GraphicsCB, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
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
					localConstantInfo.Value = m_Device->GetFrameDataChain()->GetFrontConstantBuffers()->Get(constantInfo.Value->GetSize());
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
					TextureTypes type = TextureTypes::TwoD;
					if (constant.Value != nullptr && !constant.Value->IsNull())
					{
						Texture* tex = constant.Value->GetPointer();

						texHandle = tex->GetHandle();
						type = tex->GetType();
					}

					GraphicsCB->SetProgramTexture(constant.Handle, type, texHandle);
				}

				for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
				{
					SubMesh& subMesh = Mesh->GetSubMeshes()[i];

					CoreDebugAssert(Categories::RenderSystem, GraphicsCB->SetMesh(subMesh.GetHandle()), "SetMesh has failed");

					const uint16 idxCount = subMesh.GetIndexCount();
					if (idxCount == 0)
						GraphicsCB->DrawArray(subMesh.GetPolygonType(), subMesh.GetVertexCount());
					else
						GraphicsCB->DrawIndexed(subMesh.GetPolygonType(), idxCount);
				}
			}
		}

		ICommandBuffer* CommandBuffer::FindOrCreateCommandBuffer(NativeCommandBufferList& List, ICommandBuffer::Types Type)
		{
			for (uint16 i = 0; i < List.GetSize(); ++i)
			{
				auto commandBuffer = List[i];

				if (commandBuffer->GetType() != Type)
					continue;

				List.RemoveAt(i);

				commandBuffer->Clear();

				return commandBuffer;
			}

			ICommandBuffer* commandBuffer = nullptr;
			CoreDebugAssert(Categories::RenderSystem, m_Device->CreateCommandBuffer(Type, commandBuffer).Wait(), "Couldn't create a native command buffer");

			List.Add(commandBuffer);

			return commandBuffer;
		}
	}
}