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
		private:
			CommandBuffer(ICommandBuffer* Buffer);

		public:
			~CommandBuffer(void);

			void SetViewport(const Vector2I& Position, const Vector2I& Size);

			void SetRenderTarget(RenderTarget* RenderTarget);

			void Clear(ClearFlags Flags, const ColorUI8& Color);

			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			void BeginEvent(const String& Label);
			void BeginEvent(const WString& Label);
			void EndEvent(void);

			void SetMarker(const String& Label);
			void SetMarker(const WString& Label);

			//public GPUFence CreateGPUFence();
			//Blit
			//Dispatch Compute

		private:
			ICommandBuffer* m_Buffer;
		};
	}
}

#endif