// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_DEVICE_H
#define I_DEVICE_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformWindow.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\GPUBuffer.h>
#include <Rendering\Window.h>
#include <Rendering\Color.h>
#include <Rendering\Vertex.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Rendering
	{
		class IDevice
		{
		public:
			enum class ClearFlags
			{
				ColorBuffer = 2,
				DepthBuffer = 4,
				AccumulationBuffer = 8,
				StencilBuffer = 16
			};

			enum class BufferUsages
			{
				StreamDraw = 0,
				StreamRead,
				StreamCopy,
				StaticDraw,
				StaticRead,
				StaticCopy,
				DynamicDraw,
				DynamicRead,
				DynamicCopy
			};

			enum class DrawModes
			{
				Lines = 0,
				LineLoop,
				LineStrip,
				Triangles,
				TriangleStrip,
				TriangleFan,
				Quads,
				QuadStrip,
				Polygon
			};

			struct SubMeshInfo
			{
			public:
				enum class VertexLayouts
				{
					Position = 2,
					Normal = 4,
					UV = 8
				};

			public:
				SubMeshInfo(void) :
					Vertex(nullptr),
					VertexCount(0),
					Indices(nullptr),
					IndexCount(0)
				{
				}

				VertexLayouts Layout;

				Vertex *Vertex;
				uint32 VertexCount;

				uint32 *Indices;
				uint32 IndexCount;
			};

			struct MeshInfo
			{
			public:
				MeshInfo(void) :
					SubMeshes(nullptr),
					SubMeshCount(0)
				{
				}

				SubMeshInfo *SubMeshes;
				uint32 SubMeshCount;
			};

		public:
			virtual ~IDevice(void)
			{
			}

			virtual bool Initialize(void) = 0;

			virtual void SetSampleCount(uint8 Count) = 0;

			virtual void SetForwardCompatible(bool Value) = 0;

			virtual void SetClearColor(Color Color) = 0;

			virtual void SetClearFlags(IDevice::ClearFlags Flags) = 0;

			virtual bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, Texture::Handle &Handle) = 0;
			virtual bool DestroyTexture2D(Texture::Handle Handle) = 0;

			virtual bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle &Handle) = 0;
			virtual bool DestroyProgram(Program::Handle Handle) = 0;

			virtual bool CreateMesh(const SubMeshInfo *Info, BufferUsages Usage, GPUBuffer::Handle &Handle) = 0;
			virtual bool DestroyMesh(GPUBuffer::Handle Handle) = 0;

			virtual bool CreateWindow(uint16 Width, uint16 Height, cstr Title, Window::Handle &Handle) = 0;
			virtual bool DestroyWindow(Window::Handle Handle) = 0;

			virtual void Clear(void) = 0;

			virtual bool BindProgram(Program::Handle Handle) = 0;

			virtual bool BindBuffer(GPUBuffer::Handle Handle) = 0;

			virtual void Draw(DrawModes Mode, uint32 Count) = 0;

			virtual void SwapBuffers(Window::Handle Handle) = 0;

			virtual void PollEvents(void) = 0;
			virtual bool WindowShouldClose(Window::Handle Handle) = 0;

			virtual cstr GetLastError(void) const = 0;
		};
	}
}

#endif