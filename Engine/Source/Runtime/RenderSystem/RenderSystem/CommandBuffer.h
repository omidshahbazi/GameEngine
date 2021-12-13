// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <RenderCommon\RenderCommon.h>
#include <RenderDevice\IDevice.h>
#include <RenderDevice\ProgramInfo.h>
#include <RenderDevice\CompiledProgramInfo.h>
#include <Containers\Delegate.h>
#include <Containers\Map.h>
#include <WindowUtility\Window.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace RenderCommon;
	using namespace RenderDevice;
	using namespace WindowUtility;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API CommandBuffer
		{
		private:
			CommandBuffer(void);

		public:
			~CommandBuffer(void);

			void SetViewport(const Vector2I& Position, const Vector2I& Size, RenderQueues Queue = RenderQueues::Default);

			void SetRenderTarget(RenderTarget* RenderTarget, RenderQueues Queue = RenderQueues::Default);

			void Clear(IDevice::ClearFlags Flags, const ColorUI8& Color, RenderQueues Queue = RenderQueues::Default);

			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			void BeginEvent(const String& Label, RenderQueues Queue = RenderQueues::Default);
			void BeginEvent(const WString& Label, RenderQueues Queue = RenderQueues::Default);
			void EndEvent(RenderQueues Queue = RenderQueues::Default);

			void SetMarker(const String& Label, RenderQueues Queue = RenderQueues::Default);
			void SetMarker(const WString& Label, RenderQueues Queue = RenderQueues::Default);

			//public GPUFence CreateGPUFence();
			//Blit
		};
	}
}

#endif