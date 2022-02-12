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

		class RenderContext;
		class RenderTarget;
		class Mesh;
		class Material;
		class Texture;

		class RENDERSYSTEM_API CommandBuffer
		{
			friend class DeviceInterface;

		public:
			typedef Vector<ICommandBuffer*> NativeCommandBufferList;

		public:
			CommandBuffer(void) :
				CommandBuffer("Command Buffer")
			{
			}

			CommandBuffer(const String& Name);

			const String& GetName(void) const
			{
				return m_Name;
			}
			void SetName(const String& Name)
			{
				m_Name = Name;
			}

			void Clear(void)
			{
				m_Buffer.Clear();
			}

			INLINE uint32 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

			bool CopyTexture(const Texture* Source, const Texture* Destination);
			bool CopyTexture(const Texture* Source, const Texture* Destination, const Vector2I& Position, const Vector2I& Size);
			bool CopyTexture(const Texture* Source, const Vector2I& SourcePosition, const Texture* Destination, const Vector2I& DestinationPosition, const Vector2I& Size);

			bool GenerateMipMap(const Texture* Texture);

			void SetRenderTarget(const RenderTarget* RenderTarget);
			bool SetViewport(const Vector2I& Position, const Vector2I& Size);
			void Clear(ClearFlags Flags, const ColorUI8& Color);

			bool DrawMesh(const Mesh* Mesh, const Matrix4F& Transform, const Material* Material);
			bool DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			bool DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			void BeginEvent(const String& Label);
			void BeginEvent(const WString& Label);
			void EndEvent(void);

			void SetMarker(const String& Label);
			void SetMarker(const WString& Label);

			//CreateGPUFence
			//BlitTexture
			//Dispatch
			//RenderTargetPool

		private:
			bool PrepareNativeBuffers(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, const RenderContext* RenderContext);

			static void InsertDrawCommand(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

		private:
			String m_Name;
			CommandBufferContainer m_Buffer;
		};
	}
}

#endif