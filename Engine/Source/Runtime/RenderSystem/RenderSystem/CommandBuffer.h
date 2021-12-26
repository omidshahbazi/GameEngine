// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <Containers\Strings.h>
#include <RenderDevice\ICommandBuffer.h>

namespace Engine
{
	namespace RenderDevice
	{
		class ICommandBuffer;
	}

	using namespace Containers;
	using namespace MathContainers;
	using namespace RenderCommon;
	using namespace RenderDevice;

	namespace RenderSystem
	{
		class RenderTarget;
		class Mesh;
		class Material;

		class RENDERSYSTEM_API CommandBuffer
		{
			friend class DeviceInterface;

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
			CommandBuffer(ICommandBuffer* NativeBuffer);

		public:
			INLINE void Clear(void)
			{
				return m_Buffer.Clear();
			}

			INLINE uint32 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

			void SetViewport(const Vector2I& Position, const Vector2I& Size);

			void SetRenderTarget(const RenderTarget* RenderTarget);

			void Clear(ClearFlags Flags, const ColorUI8& Color);

			void DrawMesh(const Mesh* Mesh, const Matrix4F& Transform, const Material* Material);
			void DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			void DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			void BeginEvent(const String& Label);
			void BeginEvent(const WString& Label);
			void EndEvent(void);

			void SetMarker(const String& Label);
			void SetMarker(const WString& Label);

			//public GPUFence CreateGPUFence();
			//Blit
			//Dispatch Compute

		private:
			ICommandBuffer* PrepareNativeBuffer(void);

			void InsertDrawCommand(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

		private:
			Buffer m_Buffer;
			ICommandBuffer* m_NativeBuffer;
		};
	}
}

#endif