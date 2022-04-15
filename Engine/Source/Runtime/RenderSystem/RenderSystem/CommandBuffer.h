// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <RenderDevice\ICommandBuffer.h>
#include <Containers\Strings.h>
#include <RenderCommon\CommandBufferContainer.h>
#include <RenderCommon\RenderState.h>

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;
	using namespace RenderCommon;
	using namespace RenderDevice;

	namespace RenderSystem
	{
		namespace Private
		{
			class FrameConstantBuffers;
		}

		using namespace Private;

		class CommandBufferFence;
		class RenderContext;
		class RenderTarget;
		class Mesh;
		class Material;
		class ComputeProgram;
		class Texture;

		class RENDERSYSTEM_API CommandBuffer
		{
			friend class DeviceInterface;

		public:
			CommandBuffer(void) :
				CommandBuffer("Command Buffer")
			{
			}

			CommandBuffer(const String& Name);

			String GetName(void) const
			{
				return m_Name.ChangeType<char8>();
			}
			void SetName(const String& Name)
			{
				m_Name = Name.ChangeType<char16>();
			}

			void Clear(void)
			{
				m_Buffer.Clear();
			}

			INLINE uint32 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

			bool CopyTexture(const Texture* Source, Texture* Destination);
			bool CopyTexture(const Texture* Source, Texture* Destination, const Vector2I& Position, const Vector2I& Size);
			bool CopyTexture(const Texture* Source, const Vector2I& SourcePosition, Texture* Destination, const Vector2I& DestinationPosition, const Vector2I& Size);

			bool BlitMaterial(const Material* Material, RenderTargets RenderTarget);
			bool BlitMaterial(const Material* Material, RenderTarget* RenderTarget);

			bool GenerateMipMap(Texture* Texture);

			bool SetRenderTarget(RenderTargets RenderTarget);
			bool SetRenderTarget(RenderTarget* RenderTarget);
			bool SetViewport(const Vector2I& Position, const Vector2I& Size);
			void Clear(ClearFlags Flags, const ColorUI8& Color);

			bool Draw(const Mesh* Mesh, const Matrix4F& Transform, const Material* Material);
			bool Draw(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			bool Draw(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			bool Dispath(const ComputeProgram* ComputeProgram, const Vector3I& ThreadGroupCount);

			void BeginEvent(const String& Label);
			void BeginEvent(const WString& Label);
			void EndEvent(void);

			void SetMarker(const String& Label);
			void SetMarker(const WString& Label);

			bool WaitForFence(CommandBufferFence* const Fence)
			{
				return WaitForFences(&Fence, 1);
			}
			bool WaitForFences(CommandBufferFence* const* Fences, uint16 Count);

			bool SignalFence(CommandBufferFence* const Fence)
			{
				return SignalFences(&Fence, 1);
			}
			bool SignalFences(CommandBufferFence* const* Fences, uint16 Count);

		private:
			bool PrepareNativeBuffers(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, Texture* DefaultTexture, const RenderContext* RenderContext);

			static void InsertDrawCommand(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, Texture* DefaultTexture, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Material* Material);
			static void InsertDispatchCommand(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, Texture* DefaultTexture, ComputeProgram* ComputeProgram, const Vector3I& ThreadGroupCount);

		private:
			WString m_Name;
			CommandBufferContainer m_Buffer;
		};
	}
}

#endif