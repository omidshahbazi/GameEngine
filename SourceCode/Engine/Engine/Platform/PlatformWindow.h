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
				None = 0x00000000L,
				Overlapped = 0x00000001L,
				Popup = 0x00000002L,
				Child = 0x00000004L,
				Visible = 0x00000010L,
				Disabled = 0x00000020L,
				ClipSiblings = 0x00000040L,
				ClipChildren = 0x00000080L,
				Minimize = 0x00000100L,
				Maximize = 0x00000200L,
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

				Caption = Border | DialogFrame,
				OverlappedWindow = Overlapped | Caption | SystemMenu | ThickFrame | MinimizeBox | MaximizeBox | Visible,
				PopupWindow = Popup | Border | SystemMenu
			};

			enum class ExtraStyles
			{
				None = 0x00000000L,

				DialogModalFrame = 0x00000001L,
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

			enum class ShowWindowStates
			{
				Restore = 0,
				Hide,
				Show,
				ShowInactive,
				ShowDefault,
				ShowMinimized,
				ShowMaximized,
				ShowInactiveMinimized,
				ShowNormal,
				ShowInactiveNormal,
				Minimize,
				Maximize
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

			enum class WindowMessages
			{
				None = 0,
				Create,
				Activate,
				BeginResizeMove,
				EndResizeMove,
				Resizing,
				Resized,
				Moving,
				Moved,
				Close,
				SetFocus,
				KillFocus,
				GetMinMaxInfo,
				KeyUp,
				KeyDown,
				MouseDown,
				MouseUp,
				MouseWheel,
				MouseLeave,
				MouseMove,
				CalculateSize,
				HitTest
			};

			enum class ResizedStates
			{
				Normal = 0,
				Minimized,
				Maximized
			};

			enum class Places
			{
				Top = 0,
				Bottom,
				TopMost,
				NoTopMost
			};

			enum class HitPoints
			{
				None = 0,
				Border,
				Caption,
				Client,
				SystemMenu,
				Top,
				TopLeft,
				TopRight,
				Left,
				Right,
				Bottom,
				BottomLeft,
				BottomRight
			};

			enum class VirtualKeys
			{
				LeftButton,
				RightButton,
				MiddleButton,
				X1Button,
				X2Button,

				Backspace,
				Tab,
				Return,
				Pause,
				Escape,
				Space,
				PageUp,
				PageDown,
				End,
				Home,
				Up,
				Down,
				Left,
				Right,
				PrintScreen,
				Insert,
				Delete,
				ContextMenu,

				Capital,
				NumLock,
				ScrollLock,

				Shift,
				LeftShift,
				RightShift,

				Control,
				LeftControl,
				RightControl,

				Alt,
				LeftAlt,
				RightAlt,

				LeftWindows,
				RightWindows,

				KeypadApostrophe,
				KeypadMinus,
				KeypadPlus,
				KeypadLeftSquareBraket,
				KeypadRightSquareBraket,
				KeypadColon,
				KeypadQuotation,
				KeypadBackSlash,
				KeypadComma,
				KeypadDot,
				KeypadForwardSlash,

				Keypad0,
				Keypad1,
				Keypad2,
				Keypad3,
				Keypad4,
				Keypad5,
				Keypad6,
				Keypad7,
				Keypad8,
				Keypad9,

				KeypadA,
				KeypadB,
				KeypadC,
				KeypadD,
				KeypadE,
				KeypadF,
				KeypadG,
				KeypadH,
				KeypadI,
				KeypadJ,
				KeypadK,
				KeypadL,
				KeypadM,
				KeypadN,
				KeypadO,
				KeypadP,
				KeypadQ,
				KeypadR,
				KeypadS,
				KeypadT,
				KeypadU,
				KeypadV,
				KeypadW,
				KeypadX,
				KeypadY,
				KeypadZ,

				Numpad0,
				Numpad1,
				Numpad2,
				Numpad3,
				Numpad4,
				Numpad5,
				Numpad6,
				Numpad7,
				Numpad8,
				Numpad9,
				NumpadAdd,
				NumpadSubtract,
				NumpadMultiply,
				NumpadDivide,
				NumpadDecimal,

				F1,
				F2,
				F3,
				F4,
				F5,
				F6,
				F7,
				F8,
				F9,
				F10,
				F11,
				F12,

				GamepadA,
				GamepadB,
				GamepadX,
				GamepadY,
				GamepadRightShoulder,
				GamepadLeftShoulder,
				GamepadLeftTrigger,
				GamepadRightTrigger,
				GamepadDPadUp,
				GamepadDPadDown,
				GamepadDPadLeft,
				GamepadDPadRight,
				GamepadMenu,
				GamepadView,
				GamepadLeftThumbstickButton,
				GamepadRightThumbstickButton,
				GamepadLeftThumbstickUp,
				GamepadLeftThumbstickDowns,
				GamepadLeftThumbstickRight,
				GamepadLeftThumbstickLeft,
				GamepadRightThumbstickUp,
				GamepadRightThumbstickDown,
				GamepadRightThumbstickRight,
				GamepadRightThumbstickLeft,

				COUNT
			};

			enum class SystemMetrics
			{
				PrimaryScreenWidth = 0,
				PrimaryScreenHeight,
				ScrollBarWidth,
				ScrollBarHeight,
				CaptionHeight,
				WindowBorderWidth,
				WindowBorderHeight,
				FixedFrameWidth,
				FixedFrameHeight,
				ScrollBarThumbBoxWidth,
				ScrollBarThumbBoxHeight,
				IconWidth,
				IconHeight,
				CursorWidth,
				CursorHeight,
				MernuBarHeight,
				FullscreenWidth,
				FullscreenHeight,
				KanjiWindowHeight,
				MousePresent,
				MouseButtonCount,
				MouseWheelPresent,
				MouseButtonsSwapped,
				ScrollBarArrowBitmapWidth,
				ScrollBarArrowBitmapHeight,
				DebugUserInstalled,
				MinimumWindowWidth,
				MinimumWindowHeight,
				CaptionButtonWidth,
				CaptionButtonHeight,
				SizingBorderWidth,
				SizingBorderHeight,
				MinimumWindowTrackWidth,
				MinimumWindowTrackHeight,
				MaximizedWindowTrackWidth,
				MaximizedWindowTrackHeight,
				DoubleClickAreaWidth,
				DoubleClickAreaHeight,
				DisplayCount
			};

			struct ActivateInfo
			{
			public:
				bool Focused;
			};

			struct KeyInfo
			{
			public:
				VirtualKeys Key;
				uint16 RepeatCount;
			};

			struct MouseInfo
			{
			public:
				VirtualKeys Key;
				uint16 X;
				uint16 Y;
				uint16 WheelDelta;
			};

			struct RectInfo
			{
			public:
				int32 Left;
				int32 Top;
				int32 Right;
				int32 Bottom;
			};

			struct ResizedInfo
			{
			public:
				ResizedStates State;
			};

			struct MinMaxSizeInfo
			{
			public:
				int32 MaxX;
				int32 MaxY;
				int32 MinWidth;
				int32 MinHeight;
				int32 MaxWidth;
				int32 MaxHeight;
			};

			struct HitTestInfo
			{
			public:
				int32 X;
				int32 Y;
				HitPoints Point;
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

			struct DisplayInfo
			{
			public:
				int32 DisplayX;
				int32 DisplayY;
				int32 DisplayWidth;
				int32 DisplayHeight;

				int32 WorkX;
				int32 WorkY;
				int32 WorkWidth;
				int32 WorkHeight;

				bool IsPrimary;
			};

			enum class DPIAwareness
			{
				Unaware = 0,
				SystemAware,
				PerMonitorAware1,
				PerMonitorAware2,
				UnawareGDIScaled
			};

		public:
			typedef size_t* WindowHandle;
			typedef size_t* ContextHandle;
			typedef size_t* WGLContextHandle;
			typedef size_t* DisplayHandle;
			typedef std::function<bool(WindowMessages, void* Argument)> Procedure;

		public:
			static bool Initialize(PlatformOS::Handle Handle, cstr ClassName);

			static WindowHandle Create(cstr ClassName, cstr Name, Procedure Procedure);
			static void Close(WindowHandle Handle);
			static void Destroy(WindowHandle Handle);

			static void ShowWindow(WindowHandle Handle, ShowWindowStates State);

			static void SetTitle(WindowHandle Handle, cstr Title);

			static void GetPosition(WindowHandle Handle, int16& X, int16& Y);
			static void SetPositionOption(WindowHandle Handle, Places Place);
			static void SetPosition(WindowHandle Handle, uint16 X, uint16 Y);
			static void GetClientPosition(WindowHandle Handle, int16& X, int16& Y);

			static void GetSize(WindowHandle Handle, uint16& Width, uint16& Height);
			static void SetSize(WindowHandle Handle, uint16 Width, uint16 Height);
			static void GetClientSize(WindowHandle Handle, uint16& Width, uint16& Height);

			static void SetStyle(WindowHandle Handle, Styles Style);
			static Styles GetStyle(WindowHandle Handle);
			static void SetExtraStyle(WindowHandle Handle, ExtraStyles Style);
			static ExtraStyles GetExtraStyle(WindowHandle Handle);

			static ShowWindowStates GetWindowState(WindowHandle Handle);

			static void Invalidate(WindowHandle Handle);

			static int32 Update(WindowHandle Handle);

			static ContextHandle GetDeviceContext(WindowHandle Handle);
			static void SwapBuffers(ContextHandle Handle, bool VSync = true);

			static int32 ChoosePixelFormat(ContextHandle Handle, const PixelFormatInfo* Info);
			static void SetPixelFormat(ContextHandle Handle, int32 Format, const PixelFormatInfo* Info);

			static WGLContextHandle CreateWGLContext(ContextHandle Handle);
			static WGLContextHandle CreateWGLARBContext(ContextHandle Handle, bool EnableDebugMode);
			static WGLContextHandle CreateWGLARBContext(ContextHandle Handle, WGLContextHandle ShareWithWGLContext, bool EnableDebugMode);
			static void DestroyWGLContext(WGLContextHandle Handle);
			static void MakeCurrentWGLContext(ContextHandle ContexHandle, WGLContextHandle WGLContextHandle);
			static void GetCurrentWGLContext(ContextHandle& ContexHandle, WGLContextHandle& WGLContextHandle);

			static void PollEvents(void);

			static bool GetKeyState(VirtualKeys Key);

			static void GetMousePosition(int32& X, int32& Y);

			static DisplayHandle GetDisplay(WindowHandle Handle);
			static bool GetDisplayInfo(DisplayHandle Handle, DisplayInfo* Info);

			static int32 GetMetric(SystemMetrics Metric);

			static void SetDPIAwareness(DPIAwareness Type);
		};
	}
}

#endif