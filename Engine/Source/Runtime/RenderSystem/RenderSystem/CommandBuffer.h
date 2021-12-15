// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <Containers\Strings.h>
#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API CommandBuffer
		{
		private:
			CommandBuffer(void);

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

			void SetMarker(const String& Label );
			void SetMarker(const WString& Label);

			//public GPUFence CreateGPUFence();
			//Blit
			//Dispatch Compute
		};
	}
}

#endif