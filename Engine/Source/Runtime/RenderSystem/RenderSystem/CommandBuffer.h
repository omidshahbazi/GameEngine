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
			class ThreadedDevice;
		}

		using namespace Private;

		class RenderContext;
		class RenderTarget;
		class Mesh;
		class Material;

		class RENDERSYSTEM_API CommandBuffer
		{
			friend class DeviceInterface;

		public:
			typedef Vector<ICommandBuffer*> NativeCommandBufferList;

		public:
			CommandBuffer(void) :
				CommandBuffer("CommandBuffer")
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

			INLINE void Clear(void)
			{
				return m_Buffer.Clear();
			}

			INLINE uint32 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

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

			//public GPUFence CreateGPUFence();
			//Blit
			//Dispatch Compute
			//copy texture

		private:
			void PrepareNativeBuffers(ThreadedDevice* Device, RenderContext* RenderContext, NativeCommandBufferList& NativeCommandBuffers);

			void InsertDrawCommand(ThreadedDevice* Device, ICommandBuffer* CopyConstantBuffersCB, ICommandBuffer* GraphicsCB, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

		private:
			String m_Name;
			CommandBufferContainer m_Buffer;
		};
	}
}

#endif