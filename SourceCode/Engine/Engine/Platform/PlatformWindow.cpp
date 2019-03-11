// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformWindow.h>
#include <Platform\PlatformMemory.h>
#include <Common\CharacterUtility.h>
#include <Common\BitwiseUtils.h>
#include <Windows.h>
#include <GL\glew.h>
#include <GL\wglew.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformWindow::WindowMessages GetWindowMessage(UINT Message)
		{
			switch (Message)
			{
			case WM_CREATE:
				return PlatformWindow::WindowMessages::Create;
			case WM_SIZE:
				return PlatformWindow::WindowMessages::Size;
			case WM_CLOSE:
				return PlatformWindow::WindowMessages::Close;
			}

			return PlatformWindow::WindowMessages::None;
		}

		class WindowProcedureAsLambda
		{
		public:
			WindowProcedureAsLambda(const PlatformWindow::Procedure &Procedure) :
				m_Procedure(Procedure)
			{ }

			static LRESULT CALLBACK Stub(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam)
			{
				WindowProcedureAsLambda *pThis = (WindowProcedureAsLambda*)GetWindowLongPtr(Handle, GWLP_USERDATA);

				bool result = false;

				if (pThis != nullptr)
					result = pThis->m_Procedure(GetWindowMessage(Message));
				else if (Message == WM_CREATE)
				{
					pThis = ReinterpretCast(WindowProcedureAsLambda*, ((CREATESTRUCT *)LParam)->lpCreateParams);
					SetWindowLongPtr(Handle, GWLP_USERDATA, (LONG_PTR)pThis);
					result = pThis->m_Procedure(GetWindowMessage(Message));
				}

				return DefWindowProc((HWND)Handle, (UINT)Message, (WPARAM)WParam, (LPARAM)LParam);
			}

		private:
			PlatformWindow::Procedure m_Procedure;
		};

		DWORD GetStyle(PlatformWindow::Styles Style)
		{
			DWORD style = 0;

			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Overlapped))
				style |= WS_OVERLAPPED;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Popup))
				style |= WS_POPUP;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Child))
				style |= WS_CHILD;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Minimize))
				style |= WS_MINIMIZE;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Visible))
				style |= WS_VISIBLE;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Disabled))
				style |= WS_DISABLED;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::ClipSiblings))
				style |= WS_CLIPSIBLINGS;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::ClipChildren))
				style |= WS_CLIPCHILDREN;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Maximize))
				style |= WS_MAXIMIZE;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Caption))
				style |= WS_CAPTION;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Border))
				style |= WS_BORDER;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::DialogFrame))
				style |= WS_DLGFRAME;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::VerticalScroll))
				style |= WS_VSCROLL;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::HorizontalScroll))
				style |= WS_HSCROLL;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::SystemMenu))
				style |= WS_SYSMENU;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::ThickFrame))
				style |= WS_THICKFRAME;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::Group))
				style |= WS_GROUP;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::TabStop))
				style |= WS_TABSTOP;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::MinimizeBox))
				style |= WS_MINIMIZEBOX;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Styles::MaximizeBox))
				style |= WS_MAXIMIZEBOX;

			return style;
		}

		BYTE GetPixelType(PlatformWindow::PixelTypes Type)
		{
			BYTE style = 0;

			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelTypes::RGBA))
				style |= PFD_TYPE_RGBA;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelTypes::ColorIndex))
				style |= PFD_TYPE_COLORINDEX;

			return style;
		}

		BYTE GetLayerType(PlatformWindow::LayerTypes Type)
		{
			BYTE style = 0;

			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::LayerTypes::MainPlane))
				style |= PFD_MAIN_PLANE;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::LayerTypes::OverlayPlane))
				style |= PFD_OVERLAY_PLANE;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::LayerTypes::UnderlayPlane))
				style |= PFD_UNDERLAY_PLANE;

			return style;
		}

		DWORD GetPixelFormat(PlatformWindow::PixelFormats Type)
		{
			DWORD style = 0;

			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::DoubleBuffer))
				style |= PFD_DOUBLEBUFFER;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::Stereo))
				style |= PFD_STEREO;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::DrawToWindow))
				style |= PFD_DRAW_TO_WINDOW;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::DrawToBitmap))
				style |= PFD_DRAW_TO_BITMAP;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::SupportGDI))
				style |= PFD_SUPPORT_GDI;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::SupportOpenGL))
				style |= PFD_SUPPORT_OPENGL;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::GenericFormat))
				style |= PFD_GENERIC_FORMAT;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::NeedPalette))
				style |= PFD_NEED_PALETTE;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::NeedSystemPalette))
				style |= PFD_NEED_SYSTEM_PALETTE;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::SwapExchange))
				style |= PFD_SWAP_EXCHANGE;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::SwapCopy))
				style |= PFD_SWAP_COPY;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::SwapLayerBuffers))
				style |= PFD_SWAP_LAYER_BUFFERS;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::GenericAccelerated))
				style |= PFD_GENERIC_ACCELERATED;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::SupportDirectDraw))
				style |= PFD_SUPPORT_DIRECTDRAW;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::Direct3DAccelerated))
				style |= PFD_DIRECT3D_ACCELERATED;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::SupportComposition))
				style |= PFD_SUPPORT_COMPOSITION;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::DepthDontCare))
				style |= PFD_DEPTH_DONTCARE;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::DoubleBufferDontCare))
				style |= PFD_DOUBLEBUFFER_DONTCARE;
			if (BitwiseUtils::IsEnabled(Type, PlatformWindow::PixelFormats::StereoDontCare))
				style |= PFD_STEREO_DONTCARE;

			return style;
		}

		void GetPixelFormatDescriptor(const PlatformWindow::PixelFormatInfo *Info, PIXELFORMATDESCRIPTOR &PFD)
		{
			PlatformMemory::Set(&PFD, 0, sizeof(PIXELFORMATDESCRIPTOR));

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

		PlatformWindow::WindowHandle PlatformWindow::Create(PlatformOS::Handle Handle, cstr Name, Styles Style, Procedure Procedure)
		{
			const int8 size = 20;
			static char className[size];
			uint32 nameSize = CharacterUtility::GetLength(Name);
			if (nameSize >= size)
				nameSize = size - 4;
			PlatformMemory::Copy(Name, className, nameSize);
			className[nameSize] = 'C';
			className[nameSize + 1] = 'L';
			className[nameSize + 2] = 'S';
			className[nameSize + 3] = '\0';

			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WindowProcedureAsLambda::Stub;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = (HINSTANCE)Handle;
			wcex.hIcon = nullptr;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = className;
			wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

			if (!RegisterClassEx(&wcex))
				return 0;

			return (PlatformWindow::WindowHandle)CreateWindow(
				className,
				Name,
				GetStyle(Style),
				CW_USEDEFAULT, CW_USEDEFAULT,
				100, 100,
				nullptr,
				nullptr,
				nullptr,
				new WindowProcedureAsLambda(Procedure));
		}

		void PlatformWindow::Destroy(WindowHandle Handle)
		{
			DestroyWindow((HWND)Handle);
		}

		void PlatformWindow::SetTitle(WindowHandle Handle, cstr Title)
		{
			SetWindowText((HWND)Handle, Title);
		}

		void PlatformWindow::SetPosition(WindowHandle Handle, uint16 X, uint16 Y)
		{
			SetWindowPos((HWND)Handle, 0, X, Y, 0, 0, SWP_NOSIZE);
		}

		void PlatformWindow::SetSize(WindowHandle Handle, uint16 Width, uint16 Height)
		{
			SetWindowPos((HWND)Handle, 0, 0, 0, Width, Height, SWP_NOREPOSITION);
		}

		void PlatformWindow::Show(WindowHandle Handle, bool Show)
		{
			ShowWindow((HWND)Handle, Show ? 1 : 0);
		}

		void PlatformWindow::Invalidate(WindowHandle Handle)
		{
			RedrawWindow((HWND)Handle, nullptr, 0, RDW_INVALIDATE);
		}

		void PlatformWindow::SetTopMost(WindowHandle Handle, bool TopMost)
		{
			SetWindowPos((HWND)Handle, TopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION);
		}

		bool PlatformWindow::DefaultProcedure(WindowMessages Message)
		{
			return false;
		}

		int32 PlatformWindow::Update(WindowHandle Handle)
		{
			return UpdateWindow((HWND)Handle);
		}

		PlatformWindow::ContextHandle PlatformWindow::GetDeviceContext(WindowHandle Handle)
		{
			return (PlatformWindow::WindowHandle)GetDC((HWND)Handle);
		}

		void PlatformWindow::SwapBuffers(ContextHandle Handle)
		{
			::SwapBuffers((HDC)Handle);
		}

		int32 PlatformWindow::ChoosePixelFormat(ContextHandle Handle, const PixelFormatInfo * Info)
		{
			static PIXELFORMATDESCRIPTOR pfd;
			GetPixelFormatDescriptor(Info, pfd);

			return ::ChoosePixelFormat((HDC)Handle, &pfd);
		}

		void PlatformWindow::SetPixelFormat(ContextHandle Handle, int32 Format, const PixelFormatInfo * Info)
		{
			static PIXELFORMATDESCRIPTOR pfd;
			GetPixelFormatDescriptor(Info, pfd);

			::SetPixelFormat((HDC)Handle, Format, &pfd);
		}

		PlatformWindow::WGLContextHandle PlatformWindow::CreateWGLContext(ContextHandle Handle)
		{
			return (WGLContextHandle)wglCreateContext((HDC)Handle);
		}

		PlatformWindow::WGLContextHandle PlatformWindow::CreateWGLARBContext(ContextHandle Handle, WGLContextHandle WGLContext, bool EnableCoreProfile)
		{
			HDC hdc = (HDC)Handle;
			HGLRC hglrc = (HGLRC)WGLContext;

			bool isARBAvailable = (wglewIsSupported("WGL_ARB_create_context") == 1);
			if (isARBAvailable)
			{
				int32 majorVersion;
				int32 minorVersion;
				glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
				glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

				int32 attribs[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
					WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
					WGL_CONTEXT_PROFILE_MASK_ARB, (EnableCoreProfile ? WGL_CONTEXT_CORE_PROFILE_BIT_ARB : 0),
					0
				};

				wglMakeCurrent(nullptr, nullptr);

				wglDeleteContext(hglrc);

				hglrc = wglCreateContextAttribsARB(hdc, 0, attribs);

				wglMakeCurrent(hdc, hglrc);

				return (WGLContextHandle)hglrc;
			}

			return 0;
		}

		void PlatformWindow::DestroyWGLContext(WGLContextHandle Handle)
		{
			wglDeleteContext((HGLRC)Handle);
		}

		void PlatformWindow::MakeWGLCurrent(ContextHandle ContexHandle, WGLContextHandle WGLContextHandle)
		{
			wglMakeCurrent((HDC)ContexHandle, (HGLRC)WGLContextHandle);
		}

		void PlatformWindow::PollEvents(void)
		{
			static MSG message;
			while (PeekMessage(&message, (HWND)nullptr, 0, 0, PM_REMOVE) != 0)
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
	}
}
#endif