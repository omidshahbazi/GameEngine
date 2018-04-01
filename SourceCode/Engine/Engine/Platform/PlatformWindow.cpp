// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformWindow.h>
#include <Platform\PlatformMemory.h>
#include <Common\StringUtils.h>
#include <Common\BitwiseUtils.h>
#include <Windows.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class WindowProcedureAsLambda
		{
		public:
			WindowProcedureAsLambda(const PlatformWindow::Procedure &Procedure) :
				m_Procedure(Procedure)
			{ }

			static LRESULT CALLBACK Stub(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam)
			{
				WindowProcedureAsLambda *pThis = (WindowProcedureAsLambda*)GetWindowLongPtr(Handle, GWLP_USERDATA);
				if (pThis)
					return pThis->m_Procedure((PlatformOS::Handle)Handle, (uint32)Message, (uint32*)wParam, (uint32*)lParam);
				else if (Message == WM_CREATE)
				{
					pThis = (WindowProcedureAsLambda*)(((CREATESTRUCT *)lParam)->lpCreateParams);
					SetWindowLongPtr(Handle, GWLP_USERDATA, (LONG_PTR)pThis);
					return pThis->m_Procedure((PlatformOS::Handle)Handle, (uint32)Message, (uint32*)wParam, (uint32*)lParam);
				}

				return DefWindowProc(Handle, Message, wParam, lParam);
			}

		private:
			PlatformWindow::Procedure m_Procedure;
		};

		DWORD GetStyle(PlatformWindow::Style Style)
		{
			DWORD style = 0;

			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Overlapped))
				style |= WS_OVERLAPPED;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Popup))
				style |= WS_POPUP;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Child))
				style |= WS_CHILD;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Minimize))
				style |= WS_MINIMIZE;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Visible))
				style |= WS_VISIBLE;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Disabled))
				style |= WS_DISABLED;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::ClipSiblings))
				style |= WS_CLIPSIBLINGS;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::ClipChildren))
				style |= WS_CLIPCHILDREN;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Maximize))
				style |= WS_MAXIMIZE;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Caption))
				style |= WS_CAPTION;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Border))
				style |= WS_BORDER;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::DialogFrame))
				style |= WS_DLGFRAME;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::VerticalScroll))
				style |= WS_VSCROLL;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::HorizontalScroll))
				style |= WS_HSCROLL;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::SystemMenu))
				style |= WS_SYSMENU;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::ThickFrame))
				style |= WS_THICKFRAME;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::Group))
				style |= WS_GROUP;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::TabStop))
				style |= WS_TABSTOP;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::MinimizeBox))
				style |= WS_MINIMIZEBOX;
			if (BitwiseUtils::IsEnabled(Style, PlatformWindow::Style::MaximizeBox))
				style |= WS_MAXIMIZEBOX;

			return WS_OVERLAPPEDWINDOW | WS_VISIBLE;

			return style;
		}

		PlatformWindow::Handle PlatformWindow::Create(PlatformOS::Handle Handle, cstr Name, Style Style, Procedure Procedure)
		{
			const int8 size = 20;
			static char className[size];
			uint32 nameSize = StringUtils::GetLength(Name);
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

			return (PlatformWindow::Handle)CreateWindow(
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

		void PlatformWindow::SetPosition(Handle Handle, uint32 X, uint32 Y)
		{
			SetWindowPos((HWND)Handle, 0, X, Y, 0, 0, SWP_NOSIZE);
		}

		void PlatformWindow::SetSize(Handle Handle, uint32 Width, uint32 Height)
		{
			SetWindowPos((HWND)Handle, 0, 0, 0, Width, Height, SWP_NOREPOSITION);
		}

		void PlatformWindow::Show(Handle Handle, bool Show)
		{
			ShowWindow((HWND)Handle, Show ? 1 : 0);
		}

		void PlatformWindow::Invalidate(Handle Handle)
		{
			RedrawWindow((HWND)Handle, nullptr, 0, RDW_INVALIDATE);
		}

		void PlatformWindow::SetTopMost(Handle Handle, bool TopMost)
		{
			SetWindowPos((HWND)Handle, TopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION);
		}

		int32 PlatformWindow::DefaultProcedure(Handle Handle, uint32 Message, uint32 *WParam, uint32 *LParam)
		{
			return DefWindowProc((HWND)Handle, (UINT)Message, (WPARAM)WParam, (LPARAM)LParam);
		}

		int32 PlatformWindow::Update(Handle Handle)
		{
			return UpdateWindow((HWND)Handle);
		}

		void PlatformWindow::PollEvents(void)
		{
			static MSG message;
			if (GetMessage(&message, (HWND)NULL, 0, 0) != 0)
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
	}
}
#endif