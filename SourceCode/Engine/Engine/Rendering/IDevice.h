// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_DEVICE_H
#define I_DEVICE_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformWindow.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\Window.h>
#include <Rendering\Color.h>

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

			enum class MeshUsages
			{
				ReadOnly = 0,
				WriteOnly,
				ReadAndWrite,
				Access ,
				BufferMapped,
				BUfferMapPointer,
				StreamDraw,
				StreamRead,
				StreamCopy,
				StaticDraw,
				StaticRead,
				StaticCopy,
				DynamicDraw,
				DynamicRead,
				DynamicCopy,
				SamplePassed
			};

		public:
			virtual ~IDevice(void)
			{
			}

			virtual bool Initialize(void) = 0;

			virtual void SetSampleCount(uint8 Count) = 0;

			virtual void SetForwardCompatible(bool Value) = 0;

			virtual void SetProfilingEnabled(bool Value) = 0;

			virtual void SetClearColor(Color Color) = 0;

			virtual void SetClearFlags(IDevice::ClearFlags Flags) = 0;

			virtual bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, Texture::Handle &Handle) = 0;
			virtual bool DestroyTexture2D(Texture::Handle Handle) = 0;

			virtual bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle &Handle) = 0;
			virtual bool DestroyProgram(Program::Handle Handle) = 0;

			virtual bool CreateWindow(uint16 Width, uint16 Height, cstr Title, Window::Handle &Handle) = 0;
			virtual bool DestroyWindow(Window::Handle Handle) = 0;

			virtual void Clear(void) = 0;

			virtual void SwapBuffers(Window::Handle Handle) = 0;

			virtual void PollEvents(void) = 0;
			virtual bool WindowShouldClose(Window::Handle Handle) = 0;

			virtual cstr GetLastError(void) const = 0;
		};
	}
}

#endif