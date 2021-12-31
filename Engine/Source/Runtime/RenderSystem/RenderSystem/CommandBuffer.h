// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <RenderDevice\ICommandBuffer.h>
#include <Containers\Strings.h>
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

		private:
			class Buffer : private Vector<byte>
			{
			public:
				Buffer(AllocatorBase* Allocator) :
					Vector<byte>(Allocator),
					m_ReadIndex(0)
				{
				}

				template<typename T>
				INLINE void Append(const T& Item)
				{
					const uint16 size = sizeof(T);

					const uint32 startIndex = Extend(size);

					PlatformMemory::Copy(ReinterpretCast(const byte*, &Item), GetData() + startIndex, size);
				}

				INLINE void Clear(void)
				{
					Vector<byte>::Clear();
				}

				INLINE uint32 GetSize(void)
				{
					return Vector<byte>::GetSize();
				}

				template<typename T>
				INLINE bool Read(T& Item)
				{
					if (m_ReadIndex >= Vector<byte>::GetSize())
						return false;

					const uint16 size = sizeof(T);

					PlatformMemory::Copy(Vector<byte>::GetData() + m_ReadIndex, ReinterpretCast(byte*, &Item), size);

					m_ReadIndex += size;

					return true;
				}

				INLINE void ResetRead(void)
				{
					m_ReadIndex = 0;
				}

			private:
				uint32 m_ReadIndex;
			};

		private:
			CommandBuffer(ThreadedDevice* Device, const String& Name);

		public:
			~CommandBuffer(void);

			INLINE void Clear(void)
			{
				return m_Buffer.Clear();
			}

			INLINE uint32 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

			bool SetViewport(const Vector2I& Position, const Vector2I& Size);

			void SetRenderTarget(const RenderTarget* RenderTarget);

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

		private:
			void PrepareNativeBuffers(RenderContext* RenderContext, NativeCommandBufferList& NativeCommandBuffers);

			void InsertDrawCommand(ICommandBuffer* CopyConstantBuffersCB, ICommandBuffer* GraphicsCB, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			ICommandBuffer* FindOrCreateCommandBuffer(NativeCommandBufferList& List, ICommandBuffer::Types Type);

		private:
			ThreadedDevice* m_Device;
			String m_Name;
			Buffer m_Buffer;
			NativeCommandBufferList m_NativeCommandBufferList;
		};
	}
}

#endif