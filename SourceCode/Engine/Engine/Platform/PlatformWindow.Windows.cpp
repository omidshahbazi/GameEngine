// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			case WM_SIZING:
				return PlatformWindow::WindowMessages::Resizing;
			case WM_SIZE:
				return PlatformWindow::WindowMessages::Resized;
			case WM_MOVING:
				return PlatformWindow::WindowMessages::Moving;
			case WM_MOVE:
				return PlatformWindow::WindowMessages::Moved;
			case WM_CLOSE:
				return PlatformWindow::WindowMessages::Close;
			case WM_SETFOCUS:
				return PlatformWindow::WindowMessages::SetFocus;
			case WM_KILLFOCUS:
				return PlatformWindow::WindowMessages::KillFocus;
			case WM_GETMINMAXINFO:
				return PlatformWindow::WindowMessages::GetMinMaxInfo;
			}

			return PlatformWindow::WindowMessages::None;
		}

		class WindowProcedureAsLambda
		{
		public:
			WindowProcedureAsLambda(const PlatformWindow::Procedure& Procedure) :
				m_Procedure(Procedure)
			{ }

			static LRESULT CALLBACK Stub(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam)
			{
				WindowProcedureAsLambda* pThis = (WindowProcedureAsLambda*)GetWindowLongPtr(Handle, GWLP_USERDATA);

				bool result = false;

				PlatformWindow::WindowMessages message = GetWindowMessage(Message);

				if (Message == WM_CREATE)
				{
					pThis = ReinterpretCast(WindowProcedureAsLambda*, ((CREATESTRUCT*)LParam)->lpCreateParams);

					SetWindowLongPtr(Handle, GWLP_USERDATA, (LONG_PTR)pThis);

					result = pThis->m_Procedure(message, nullptr);
				}
				else if (pThis != nullptr)
				{
					if (Message == WM_GETMINMAXINFO)
					{
						PlatformWindow::MinMaxSizeInfo info;
						PlatformMemory::Set(&info, 0, 1);

						if ((result = pThis->m_Procedure(message, &info)))
						{
							MINMAXINFO* minMaxInfo = ReinterpretCast(MINMAXINFO*, LParam);

							minMaxInfo->ptMinTrackSize.x = info.MinWidth;
							minMaxInfo->ptMinTrackSize.y = info.MinHeight;
							minMaxInfo->ptMaxTrackSize.x = info.MaxWidth;
							minMaxInfo->ptMaxTrackSize.y = info.MaxHeight;
						}
					}
					else
						result = pThis->m_Procedure(message, nullptr);
				}

				return DefWindowProc((HWND)Handle, (UINT)Message, (WPARAM)WParam, (LPARAM)LParam);
			}

		private:
			PlatformWindow::Procedure m_Procedure;
		};

		DWORD GetStyleMask(PlatformWindow::Styles Style)
		{
			DWORD style = 0;

			if ((Style & PlatformWindow::Styles::Overlapped) == PlatformWindow::Styles::Overlapped)
				style |= WS_OVERLAPPED;
			if ((Style & PlatformWindow::Styles::Popup) == PlatformWindow::Styles::Popup)
				style |= WS_POPUP;
			if ((Style & PlatformWindow::Styles::Child) == PlatformWindow::Styles::Child)
				style |= WS_CHILD;
			if ((Style & PlatformWindow::Styles::Visible) == PlatformWindow::Styles::Visible)
				style |= WS_VISIBLE;
			if ((Style & PlatformWindow::Styles::Caption) == PlatformWindow::Styles::Caption)
				style |= WS_CAPTION;
			if ((Style & PlatformWindow::Styles::Disabled) == PlatformWindow::Styles::Disabled)
				style |= WS_DISABLED;
			if ((Style & PlatformWindow::Styles::ClipSiblings) == PlatformWindow::Styles::ClipSiblings)
				style |= WS_CLIPSIBLINGS;
			if ((Style & PlatformWindow::Styles::ClipChildren) == PlatformWindow::Styles::ClipChildren)
				style |= WS_CLIPCHILDREN;
			if ((Style & PlatformWindow::Styles::Minimize) == PlatformWindow::Styles::Minimize)
				style |= WS_MINIMIZE;
			if ((Style & PlatformWindow::Styles::Maximize) == PlatformWindow::Styles::Maximize)
				style |= WS_MAXIMIZE;
			if ((Style & PlatformWindow::Styles::Border) == PlatformWindow::Styles::Border)
				style |= WS_BORDER;
			if ((Style & PlatformWindow::Styles::DialogFrame) == PlatformWindow::Styles::DialogFrame)
				style |= WS_DLGFRAME;
			if ((Style & PlatformWindow::Styles::VerticalScroll) == PlatformWindow::Styles::VerticalScroll)
				style |= WS_VSCROLL;
			if ((Style & PlatformWindow::Styles::HorizontalScroll) == PlatformWindow::Styles::HorizontalScroll)
				style |= WS_HSCROLL;
			if ((Style & PlatformWindow::Styles::SystemMenu) == PlatformWindow::Styles::SystemMenu)
				style |= WS_SYSMENU;
			if ((Style & PlatformWindow::Styles::ThickFrame) == PlatformWindow::Styles::ThickFrame)
				style |= WS_THICKFRAME;
			if ((Style & PlatformWindow::Styles::Group) == PlatformWindow::Styles::Group)
				style |= WS_GROUP;
			if ((Style & PlatformWindow::Styles::TabStop) == PlatformWindow::Styles::TabStop)
				style |= WS_TABSTOP;
			if ((Style & PlatformWindow::Styles::MinimizeBox) == PlatformWindow::Styles::MinimizeBox)
				style |= WS_MINIMIZEBOX;
			if ((Style & PlatformWindow::Styles::MaximizeBox) == PlatformWindow::Styles::MaximizeBox)
				style |= WS_MAXIMIZEBOX;

			return style;
		}

		DWORD GetExtraStyleMax(PlatformWindow::ExtraStyles Style)
		{
			DWORD style = 0;

			if ((Style & PlatformWindow::ExtraStyles::DialogModalFrame) == PlatformWindow::ExtraStyles::DialogModalFrame)
				style |= WS_EX_DLGMODALFRAME;
			if ((Style & PlatformWindow::ExtraStyles::NoParentNotify) == PlatformWindow::ExtraStyles::NoParentNotify)
				style |= WS_EX_NOPARENTNOTIFY;
			if ((Style & PlatformWindow::ExtraStyles::TopMost) == PlatformWindow::ExtraStyles::TopMost)
				style |= WS_EX_TOPMOST;
			if ((Style & PlatformWindow::ExtraStyles::AcceptFiles) == PlatformWindow::ExtraStyles::AcceptFiles)
				style |= WS_EX_ACCEPTFILES;
			if ((Style & PlatformWindow::ExtraStyles::Transparent) == PlatformWindow::ExtraStyles::Transparent)
				style |= WS_EX_TRANSPARENT;
			if ((Style & PlatformWindow::ExtraStyles::MDIChild) == PlatformWindow::ExtraStyles::MDIChild)
				style |= WS_EX_MDICHILD;
			if ((Style & PlatformWindow::ExtraStyles::ToolWindow) == PlatformWindow::ExtraStyles::ToolWindow)
				style |= WS_EX_TOOLWINDOW;
			if ((Style & PlatformWindow::ExtraStyles::WindowEdge) == PlatformWindow::ExtraStyles::WindowEdge)
				style |= WS_EX_WINDOWEDGE;
			if ((Style & PlatformWindow::ExtraStyles::ClientEdge) == PlatformWindow::ExtraStyles::ClientEdge)
				style |= WS_EX_CLIENTEDGE;
			if ((Style & PlatformWindow::ExtraStyles::ContextHelp) == PlatformWindow::ExtraStyles::ContextHelp)
				style |= WS_EX_CONTEXTHELP;
			if ((Style & PlatformWindow::ExtraStyles::Right) == PlatformWindow::ExtraStyles::Right)
				style |= WS_EX_RIGHT;
			if ((Style & PlatformWindow::ExtraStyles::Left) == PlatformWindow::ExtraStyles::Left)
				style |= WS_EX_LEFT;
			if ((Style & PlatformWindow::ExtraStyles::RTLReading) == PlatformWindow::ExtraStyles::RTLReading)
				style |= WS_EX_RTLREADING;
			if ((Style & PlatformWindow::ExtraStyles::LTRReading) == PlatformWindow::ExtraStyles::LTRReading)
				style |= WS_EX_LTRREADING;
			if ((Style & PlatformWindow::ExtraStyles::LeftScrollBar) == PlatformWindow::ExtraStyles::LeftScrollBar)
				style |= WS_EX_LEFTSCROLLBAR;
			if ((Style & PlatformWindow::ExtraStyles::RightScrollBar) == PlatformWindow::ExtraStyles::RightScrollBar)
				style |= WS_EX_RIGHTSCROLLBAR;
			if ((Style & PlatformWindow::ExtraStyles::ControlParent) == PlatformWindow::ExtraStyles::ControlParent)
				style |= WS_EX_CONTROLPARENT;
			if ((Style & PlatformWindow::ExtraStyles::StaticEdge) == PlatformWindow::ExtraStyles::StaticEdge)
				style |= WS_EX_STATICEDGE;
			if ((Style & PlatformWindow::ExtraStyles::AppWindow) == PlatformWindow::ExtraStyles::AppWindow)
				style |= WS_EX_APPWINDOW;

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

			if ((Type & PlatformWindow::PixelFormats::DoubleBuffer) == PlatformWindow::PixelFormats::DoubleBuffer)
				style |= PFD_DOUBLEBUFFER;
			if ((Type & PlatformWindow::PixelFormats::Stereo) == PlatformWindow::PixelFormats::Stereo)
				style |= PFD_STEREO;
			if ((Type & PlatformWindow::PixelFormats::DrawToWindow) == PlatformWindow::PixelFormats::DrawToWindow)
				style |= PFD_DRAW_TO_WINDOW;
			if ((Type & PlatformWindow::PixelFormats::DrawToBitmap) == PlatformWindow::PixelFormats::DrawToBitmap)
				style |= PFD_DRAW_TO_BITMAP;
			if ((Type & PlatformWindow::PixelFormats::SupportGDI) == PlatformWindow::PixelFormats::SupportGDI)
				style |= PFD_SUPPORT_GDI;
			if ((Type & PlatformWindow::PixelFormats::SupportOpenGL) == PlatformWindow::PixelFormats::SupportOpenGL)
				style |= PFD_SUPPORT_OPENGL;
			if ((Type & PlatformWindow::PixelFormats::GenericFormat) == PlatformWindow::PixelFormats::GenericFormat)
				style |= PFD_GENERIC_FORMAT;
			if ((Type & PlatformWindow::PixelFormats::NeedPalette) == PlatformWindow::PixelFormats::NeedPalette)
				style |= PFD_NEED_PALETTE;
			if ((Type & PlatformWindow::PixelFormats::NeedSystemPalette) == PlatformWindow::PixelFormats::NeedSystemPalette)
				style |= PFD_NEED_SYSTEM_PALETTE;
			if ((Type & PlatformWindow::PixelFormats::SwapExchange) == PlatformWindow::PixelFormats::SwapExchange)
				style |= PFD_SWAP_EXCHANGE;
			if ((Type & PlatformWindow::PixelFormats::SwapCopy) == PlatformWindow::PixelFormats::SwapCopy)
				style |= PFD_SWAP_COPY;
			if ((Type & PlatformWindow::PixelFormats::SwapLayerBuffers) == PlatformWindow::PixelFormats::SwapLayerBuffers)
				style |= PFD_SWAP_LAYER_BUFFERS;
			if ((Type & PlatformWindow::PixelFormats::GenericAccelerated) == PlatformWindow::PixelFormats::GenericAccelerated)
				style |= PFD_GENERIC_ACCELERATED;
			if ((Type & PlatformWindow::PixelFormats::SupportDirectDraw) == PlatformWindow::PixelFormats::SupportDirectDraw)
				style |= PFD_SUPPORT_DIRECTDRAW;
			if ((Type & PlatformWindow::PixelFormats::Direct3DAccelerated) == PlatformWindow::PixelFormats::Direct3DAccelerated)
				style |= PFD_DIRECT3D_ACCELERATED;
			if ((Type & PlatformWindow::PixelFormats::SupportComposition) == PlatformWindow::PixelFormats::SupportComposition)
				style |= PFD_SUPPORT_COMPOSITION;
			if ((Type & PlatformWindow::PixelFormats::DepthDontCare) == PlatformWindow::PixelFormats::DepthDontCare)
				style |= PFD_DEPTH_DONTCARE;
			if ((Type & PlatformWindow::PixelFormats::DoubleBufferDontCare) == PlatformWindow::PixelFormats::DoubleBufferDontCare)
				style |= PFD_DOUBLEBUFFER_DONTCARE;
			if ((Type & PlatformWindow::PixelFormats::StereoDontCare) == PlatformWindow::PixelFormats::StereoDontCare)
				style |= PFD_STEREO_DONTCARE;

			return style;
		}

		void GetPixelFormatDescriptor(const PlatformWindow::PixelFormatInfo* Info, PIXELFORMATDESCRIPTOR& PFD)
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
				GetStyleMask(Style),
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

		void PlatformWindow::GetPosition(WindowHandle Handle, int16& X, int16& Y)
		{
			RECT rect;
			GetWindowRect((HWND)Handle, &rect);
			X = rect.left;
			Y = rect.top;
		}

		void PlatformWindow::SetPosition(WindowHandle Handle, uint16 X, uint16 Y)
		{
			SetWindowPos((HWND)Handle, 0, X, Y, 0, 0, SWP_NOSIZE);
		}

		void PlatformWindow::GetClientPosition(WindowHandle Handle, int16& X, int16& Y)
		{
			int32 borderSizeX = GetSystemMetrics(SM_CXSIZEFRAME);
			int32 titleBarHeight = GetSystemMetrics(SM_CYBORDER);

			GetPosition(Handle, X, Y);
			X += borderSizeX;
			Y += titleBarHeight;
		}

		void PlatformWindow::GetSize(WindowHandle Handle, uint16& Width, uint16& Height)
		{
			RECT rect;
			GetWindowRect((HWND)Handle, &rect);
			Width = rect.right - rect.left;
			Height = rect.bottom - rect.top;
		}

		void PlatformWindow::SetSize(WindowHandle Handle, uint16 Width, uint16 Height)
		{
			SetWindowPos((HWND)Handle, 0, 0, 0, Width, Height, SWP_NOMOVE);
		}

		void PlatformWindow::GetClientSize(WindowHandle Handle, uint16& Width, uint16& Height)
		{
			RECT rect;
			GetClientRect((HWND)Handle, &rect);
			Width = rect.right - rect.left;
			Height = rect.bottom - rect.top;
		}

		void PlatformWindow::SetStyle(WindowHandle Handle, Styles Style)
		{
			SetWindowLongPtr((HWND)Handle, GWL_STYLE, GetStyleMask(Style));
			//ShowWindow()
		}

		PlatformWindow::Styles PlatformWindow::GetStyle(WindowHandle Handle)
		{
			long wStyle = GetWindowLongPtr((HWND)Handle, GWL_STYLE);

			Styles style = (Styles)0;

			if ((wStyle & WS_OVERLAPPED) == WS_OVERLAPPED)
				style |= PlatformWindow::Styles::Overlapped;
			if ((wStyle & WS_POPUP) == WS_POPUP)
				style |= PlatformWindow::Styles::Popup;
			if ((wStyle & WS_CHILD) == WS_CHILD)
				style |= PlatformWindow::Styles::Child;
			if ((wStyle & WS_VISIBLE) == WS_VISIBLE)
				style |= PlatformWindow::Styles::Visible;
			if ((wStyle & WS_CAPTION) == WS_CAPTION)
				style |= PlatformWindow::Styles::Caption;
			if ((wStyle & WS_DISABLED) == WS_DISABLED)
				style |= PlatformWindow::Styles::Disabled;
			if ((wStyle & WS_CLIPSIBLINGS) == WS_CLIPSIBLINGS)
				style |= PlatformWindow::Styles::ClipSiblings;
			if ((wStyle & WS_CLIPCHILDREN) == WS_CLIPCHILDREN)
				style |= PlatformWindow::Styles::ClipChildren;
			if ((wStyle & WS_MINIMIZE) == WS_MINIMIZE)
				style |= PlatformWindow::Styles::Minimize;
			if ((wStyle & WS_MAXIMIZE) == WS_MAXIMIZE)
				style |= PlatformWindow::Styles::Maximize;
			if ((wStyle & WS_BORDER) == WS_BORDER)
				style |= PlatformWindow::Styles::Border;
			if ((wStyle & WS_DLGFRAME) == WS_DLGFRAME)
				style |= PlatformWindow::Styles::DialogFrame;
			if ((wStyle & WS_VSCROLL) == WS_VSCROLL)
				style |= PlatformWindow::Styles::VerticalScroll;
			if ((wStyle & WS_HSCROLL) == WS_HSCROLL)
				style |= PlatformWindow::Styles::HorizontalScroll;
			if ((wStyle & WS_SYSMENU) == WS_SYSMENU)
				style |= PlatformWindow::Styles::SystemMenu;
			if ((wStyle & WS_THICKFRAME) == WS_THICKFRAME)
				style |= PlatformWindow::Styles::ThickFrame;
			if ((wStyle & WS_GROUP) == WS_GROUP)
				style |= PlatformWindow::Styles::Group;
			if ((wStyle & WS_TABSTOP) == WS_TABSTOP)
				style |= PlatformWindow::Styles::TabStop;
			if ((wStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX)
				style |= PlatformWindow::Styles::MinimizeBox;
			if ((wStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX)
				style |= PlatformWindow::Styles::MaximizeBox;

			return style;
		}

		void PlatformWindow::SetExtraStyle(WindowHandle Handle, ExtraStyles Style)
		{
			SetWindowLongPtr((HWND)Handle, GWL_EXSTYLE, GetExtraStyleMax(Style));
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

		int32 PlatformWindow::ChoosePixelFormat(ContextHandle Handle, const PixelFormatInfo* Info)
		{
			static PIXELFORMATDESCRIPTOR pfd;
			GetPixelFormatDescriptor(Info, pfd);

			return ::ChoosePixelFormat((HDC)Handle, &pfd);
		}

		void PlatformWindow::SetPixelFormat(ContextHandle Handle, int32 Format, const PixelFormatInfo* Info)
		{
			static PIXELFORMATDESCRIPTOR pfd;
			GetPixelFormatDescriptor(Info, pfd);

			::SetPixelFormat((HDC)Handle, Format, &pfd);
		}

		PlatformWindow::WGLContextHandle PlatformWindow::CreateWGLContext(ContextHandle Handle)
		{
			return (WGLContextHandle)wglCreateContext((HDC)Handle);
		}

		PlatformWindow::WGLContextHandle PlatformWindow::CreateWGLARBContext(ContextHandle Handle, bool EnableDebugMode)
		{
			return CreateWGLARBContext(Handle, 0, EnableDebugMode);
		}

		PlatformWindow::WGLContextHandle PlatformWindow::CreateWGLARBContext(ContextHandle Handle, WGLContextHandle ShareWithWGLContext, bool EnableDebugMode)
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

				int32 majorVersion;
				int32 minorVersion;
				glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
				glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

				int32 attribs[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
					WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
					WGL_CONTEXT_PROFILE_MASK_ARB, (EnableDebugMode ? WGL_CONTEXT_DEBUG_BIT_ARB : 0),
					WGL_CONTEXT_FLAGS_ARB, (EnableDebugMode ? WGL_CONTEXT_DEBUG_BIT_ARB : 0),
					0
				};

				hglrc = wglCreateContextAttribsARB(hdc, shareWithHGLRC, attribs);
			}

			if (shareWithHGLRC != 0)
				wglShareLists(shareWithHGLRC, hglrc);

			return (WGLContextHandle)hglrc;
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