// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformGL.h>
#include <Platform\PlatformMemory.h>
#include <Common\BitwiseUtils.h>
#include <GLEW\include\GL\glew.h>
#include <GLEW\include\GL\wglew.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
#define SET_IF_ENABLED(WindowStyleVariable, WindowStyle, StyleVariable, Style) \
		if ((WindowStyleVariable & WindowStyle) == WindowStyle) \
			StyleVariable |= Style;

		BYTE GetPixelType(PlatformGL::PixelTypes Type)
		{
			BYTE style = 0;

			SET_IF_ENABLED(Type, PlatformGL::PixelTypes::RGBA, style, PFD_TYPE_RGBA);
			SET_IF_ENABLED(Type, PlatformGL::PixelTypes::ColorIndex, style, PFD_TYPE_COLORINDEX);

			return style;
		}

		BYTE GetLayerType(PlatformGL::LayerTypes Type)
		{
			BYTE style = 0;

			SET_IF_ENABLED(Type, PlatformGL::LayerTypes::MainPlane, style, PFD_MAIN_PLANE);
			SET_IF_ENABLED(Type, PlatformGL::LayerTypes::OverlayPlane, style, PFD_OVERLAY_PLANE);
			SET_IF_ENABLED(Type, PlatformGL::LayerTypes::UnderlayPlane, style, PFD_UNDERLAY_PLANE);

			return style;
		}

		DWORD GetPixelFormat(PlatformGL::PixelFormats Type)
		{
			DWORD style = 0;

			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::DoubleBuffer, style, PFD_DOUBLEBUFFER);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::Stereo, style, PFD_STEREO);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::DrawToWindow, style, PFD_DRAW_TO_WINDOW);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::DrawToBitmap, style, PFD_DRAW_TO_BITMAP);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::SupportGDI, style, PFD_SUPPORT_GDI);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::SupportOpenGL, style, PFD_SUPPORT_OPENGL);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::GenericFormat, style, PFD_GENERIC_FORMAT);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::NeedPalette, style, PFD_NEED_PALETTE);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::NeedSystemPalette, style, PFD_NEED_SYSTEM_PALETTE);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::SwapExchange, style, PFD_SWAP_EXCHANGE);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::SwapCopy, style, PFD_SWAP_COPY);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::SwapLayerBuffers, style, PFD_SWAP_LAYER_BUFFERS);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::GenericAccelerated, style, PFD_GENERIC_ACCELERATED);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::SupportDirectDraw, style, PFD_SUPPORT_DIRECTDRAW);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::Direct3DAccelerated, style, PFD_DIRECT3D_ACCELERATED);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::SupportComposition, style, PFD_SUPPORT_COMPOSITION);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::DepthDontCare, style, PFD_DEPTH_DONTCARE);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::DoubleBufferDontCare, style, PFD_DOUBLEBUFFER_DONTCARE);
			SET_IF_ENABLED(Type, PlatformGL::PixelFormats::StereoDontCare, style, PFD_STEREO_DONTCARE);

			return style;
		}

		void GetPixelFormatDescriptor(const PlatformGL::PixelFormatInfo* Info, PIXELFORMATDESCRIPTOR& PFD)
		{
			PlatformMemory::Set(&PFD, 0, 1);

			PFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			PFD.nVersion = 1;
			PFD.dwFlags = GetPixelFormat(Info->PixelFormat);
			PFD.iPixelType = GetPixelType(Info->PixelType);
			PFD.cColorBits = Info->ColorSize;
			PFD.cRedBits = 0;
			PFD.cRedShift = 0;
			PFD.cGreenBits = 0;
			PFD.cGreenShift = 0;
			PFD.cBlueBits = 0;
			PFD.cBlueShift = 0;
			PFD.cAlphaBits = 0;
			PFD.cAlphaShift = 0;
			PFD.cAccumBits = 0;
			PFD.cAccumRedBits = 0;
			PFD.cAccumGreenBits = 0;
			PFD.cAccumBlueBits = 0;
			PFD.cAccumAlphaBits = 0;
			PFD.cDepthBits = Info->DepthSize;
			PFD.cStencilBits = Info->StencilSize;
			PFD.cAuxBuffers = 0;
			PFD.iLayerType = GetLayerType(Info->LayerType);
			PFD.bReserved = 0;
			PFD.dwLayerMask = 0;
			PFD.dwVisibleMask = 0;
			PFD.dwDamageMask = 0;
		}

		PlatformGL::ContextHandle PlatformGL::GetDeviceContext(PlatformWindow::WindowHandle Handle)
		{
			return (PlatformWindow::WindowHandle)GetDC((HWND)Handle);
		}

		void PlatformGL::SwapBuffers(ContextHandle Handle, bool VSync)
		{
			wglSwapIntervalEXT(VSync ? 1 : 0);
			::SwapBuffers((HDC)Handle);
		}

		int32 PlatformGL::ChoosePixelFormat(ContextHandle Handle, const PixelFormatInfo* Info)
		{
			static PIXELFORMATDESCRIPTOR pfd;
			GetPixelFormatDescriptor(Info, pfd);

			return ::ChoosePixelFormat((HDC)Handle, &pfd);
		}

		void PlatformGL::SetPixelFormat(ContextHandle Handle, int32 Format, const PixelFormatInfo* Info)
		{
			static PIXELFORMATDESCRIPTOR pfd;
			GetPixelFormatDescriptor(Info, pfd);

			::SetPixelFormat((HDC)Handle, Format, &pfd);
		}

		PlatformGL::WGLContextHandle PlatformGL::CreateWGLContext(ContextHandle Handle)
		{
			return (WGLContextHandle)wglCreateContext((HDC)Handle);
		}

		PlatformGL::WGLContextHandle PlatformGL::CreateWGLARBContext(ContextHandle Handle, bool EnableDebugMode)
		{
			return CreateWGLARBContext(Handle, 0, EnableDebugMode);
		}

		PlatformGL::WGLContextHandle PlatformGL::CreateWGLARBContext(ContextHandle Handle, WGLContextHandle ShareWithWGLContext, bool EnableDebugMode)
		{
			HDC hdc = (HDC)Handle;
			HGLRC hglrc = wglCreateContext(hdc);;
			HGLRC shareWithHGLRC = (HGLRC)ShareWithWGLContext;

			wglMakeCurrent(hdc, hglrc);

			static bool initialized = false;
			if (!initialized)
			{
				initialized = true;
				glewExperimental = true;
				if (glewInit() != GLEW_OK)
					return 0;
			}

			if (wglewIsSupported("WGL_ARB_create_context") == 1)
			{
				wglMakeCurrent(0, 0);
				wglDeleteContext(hglrc);

				int32 attribs[5];
				uint8 index = 0;

				attribs[index++] = WGL_CONTEXT_PROFILE_MASK_ARB;
				attribs[index++] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

				if (EnableDebugMode)
				{
					attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
					attribs[index++] = WGL_CONTEXT_DEBUG_BIT_ARB;
				}

				attribs[index++] = 0;

				hglrc = wglCreateContextAttribsARB(hdc, shareWithHGLRC, attribs);
			}

			if (shareWithHGLRC != 0)
				wglShareLists(shareWithHGLRC, hglrc);

			return (WGLContextHandle)hglrc;
		}

		void PlatformGL::DestroyWGLContext(WGLContextHandle Handle)
		{
			wglDeleteContext((HGLRC)Handle);
		}

		void PlatformGL::MakeCurrentWGLContext(ContextHandle ContextHandle, WGLContextHandle WGLContextHandle)
		{
			wglMakeCurrent((HDC)ContextHandle, (HGLRC)WGLContextHandle);
		}

		void PlatformGL::GetCurrentWGLContext(ContextHandle& ContextHandle, WGLContextHandle& WGLContextHandle)
		{
			ContextHandle = (PlatformGL::ContextHandle)wglGetCurrentDC();
			WGLContextHandle = (PlatformGL::WGLContextHandle)wglGetCurrentContext();
		}
	}
}
#endif