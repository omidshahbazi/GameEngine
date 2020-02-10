// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformOS.h>
#include <functional>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformWindow
		{
		public:
			enum class Styles
			{
				Overlapped = 0x00000000L,
				Popup = 0x00000002L,
				Child = 0x00000004L,
				Minimize = 0x00000008L,
				Visible = 0x00000010L,
				Disabled = 0x00000020L,
				ClipSiblings = 0x00000040L,
				ClipChildren = 0x00000080L,
				Maximize = 0x00000100L,
				Caption = 0x00000200L,
				Border = 0x00000400L,
				DialogFrame = 0x00000800L,
				VerticalScroll = 0x00001000L,
				HorizontalScroll = 0x00002000L,
				SystemMenu = 0x00004000L,
				ThickFrame = 0x00008000L,
				Group = 0x00010000L,
				TabStop = 0x00020000L,
				MinimizeBox = 0x00040000L,
				MaximizeBox = 0x00080000L,

				OverlappedWindow = Overlapped | Caption | SystemMenu | ThickFrame | MinimizeBox | MaximizeBox | Visible,
				PopupWindow = Popup | Border | SystemMenu
			};

			enum class ExtraStyles
			{
				DialogModalFrame = 0x00000000L,
				NoParentNotify = 0x00000002L,
				TopMost = 0x00000004L,
				AcceptFiles = 0x00000008L,
				Transparent = 0x00000010L,

				MDIChild = 0x00000020L,
				ToolWindow = 0x00000040L,
				WindowEdge = 0x00000080L,
				ClientEdge = 0x00000100L,
				ContextHelp = 0x00000200L,

				Right = 0x00000400L,
				Left = 0x00000800L,
				RTLReading = 0x00001000L,
				LTRReading = 0x00002000L,
				LeftScrollBar = 0x00004000L,
				RightScrollBar = 0x00008000L,

				ControlParent = 0x00010000L,
				StaticEdge = 0x00020000L,
				AppWindow = 0x00004000L,

				OverlappedWindow = WindowEdge | ClientEdge,
				PaletteWindow = WindowEdge | ToolWindow | TopMost
			};

			enum class WindowMessages
			{
				None,
				Create,
				Size,
				Move,
				Close,
				SetFocus,
				KillFocus
			};

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
			typedef size_t* WindowHandle;
			typedef size_t* ContextHandle;
			typedef size_t* WGLContextHandle;
			typedef std::function<bool(WindowMessages)> Procedure;

		public:
			static WindowHandle Create(PlatformOS::Handle Handle, cstr Name, Styles Style, Procedure Procedure);
			static void Destroy(WindowHandle Handle);

			static void SetTitle(WindowHandle Handle, cstr Title);
			static void GetPosition(WindowHandle Handle, int16& X, int16& Y);
			static void SetPosition(WindowHandle Handle, uint16 X, uint16 Y);
			static void GetClientPosition(WindowHandle Handle, int16& X, int16& Y);
			static void GetSize(WindowHandle Handle, uint16& Width, uint16& Height);
			static void SetSize(WindowHandle Handle, uint16 Width, uint16 Height);
			static void GetClientSize(WindowHandle Handle, uint16& Width, uint16& Height);
			static void SetStyle(WindowHandle Handle, Styles Style);
			static void SetExtraStyle(WindowHandle Handle, ExtraStyles Style);
			static void Invalidate(WindowHandle Handle);
			static void SetTopMost(WindowHandle Handle, bool TopMost);

			static bool DefaultProcedure(WindowMessages Message);

			static int32 Update(WindowHandle Handle);

			static ContextHandle GetDeviceContext(WindowHandle Handle);
			static void SwapBuffers(ContextHandle Handle);

			static int32 ChoosePixelFormat(ContextHandle Handle, const PixelFormatInfo* Info);
			static void SetPixelFormat(ContextHandle Handle, int32 Format, const PixelFormatInfo* Info);

			static WGLContextHandle CreateWGLContext(ContextHandle Handle);
			static WGLContextHandle CreateWGLARBContext(ContextHandle Handle, WGLContextHandle WGLContext, bool EnableDebugMode);
			static void DestroyWGLContext(WGLContextHandle Handle);
			static void MakeWGLCurrent(ContextHandle ContexHandle, WGLContextHandle WGLContextHandle);

			static void PollEvents(void);
		};
	}
}

#endif