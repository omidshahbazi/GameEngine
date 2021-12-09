// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_GL_H
#define PLATFORM_GL_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformWindow.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
#define USE_HIGH_PERFORMANCE_GPU() \
		extern "C" \
		{ \
			EXPORT_API Engine::uint64 NvOptimusEnablement = 0x00000001; \
			EXPORT_API Engine::uint64 AmdPowerXpressRequestHighPerformance = 1; \
		}

		class PLATFORM_API PlatformGL
		{
		public:
			enum class PixelTypes
			{
				RGBA = 0,
				ColorIndex = 1
			};

			enum class LayerTypes
			{
				MainPlane = 0,
				OverlayPlane = 1,
				UnderlayPlane = -1
			};

			enum class PixelFormats
			{
				DoubleBuffer = 0x00000001,
				Stereo = 0x00000002,
				DrawToWindow = 0x00000004,
				DrawToBitmap = 0x00000008,
				SupportGDI = 0x00000010,
				SupportOpenGL = 0x00000020,
				GenericFormat = 0x00000040,
				NeedPalette = 0x00000080,
				NeedSystemPalette = 0x00000100,
				SwapExchange = 0x00000200,
				SwapCopy = 0x00000400,
				SwapLayerBuffers = 0x00000800,
				GenericAccelerated = 0x00001000,
				SupportDirectDraw = 0x00002000,
				Direct3DAccelerated = 0x00004000,
				SupportComposition = 0x00008000,
				DepthDontCare = 0x20000000,
				DoubleBufferDontCare = 0x40000000,
				StereoDontCare = 0x80000000
			};

			struct PixelFormatInfo
			{
				PixelFormats PixelFormat;
				PixelTypes PixelType;
				uint8 ColorSize;
				uint8 DepthSize;
				uint8 StencilSize;
				LayerTypes LayerType;
			};

		public:
			typedef size_t* ContextHandle;
			typedef size_t* WGLContextHandle;

		public:
			static ContextHandle GetDeviceContext(PlatformWindow::WindowHandle Handle);
			static void SwapBuffers(ContextHandle Handle, bool VSync = true);

			static int32 ChoosePixelFormat(ContextHandle Handle, const PixelFormatInfo* Info);
			static void SetPixelFormat(ContextHandle Handle, int32 Format, const PixelFormatInfo* Info);

			static WGLContextHandle CreateWGLContext(ContextHandle Handle);
			static WGLContextHandle CreateWGLARBContext(ContextHandle Handle, bool EnableDebugMode);
			static WGLContextHandle CreateWGLARBContext(ContextHandle Handle, WGLContextHandle ShareWithWGLContext, bool EnableDebugMode);
			static void DestroyWGLContext(WGLContextHandle Handle);
			static void MakeCurrentWGLContext(ContextHandle ContexHandle, WGLContextHandle WGLContextHandle);
			static void GetCurrentWGLContext(ContextHandle& ContexHandle, WGLContextHandle& WGLContextHandle);
		};
	}
}

#endif