// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformWindow.h>
#include <Platform\PlatformMemory.h>
#include <Common\CharacterUtility.h>
#include <Common\BitwiseUtils.h>
#include <Windows.h>
#include <windowsx.h>
#include <GL\glew.h>
#include <GL\wglew.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
#define SET_IF_ENABLED(WindowStyleVariable, WindowStyle, StyleVariable, Style) \
		if ((WindowStyleVariable & WindowStyle) == WindowStyle) \
			StyleVariable |= Style;

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
			case WM_KEYUP:
				return PlatformWindow::WindowMessages::KeyUp;
			case WM_KEYDOWN:
				return PlatformWindow::WindowMessages::KeyDown;
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
				return PlatformWindow::WindowMessages::MouseDown;
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				return PlatformWindow::WindowMessages::MouseUp;
			case WM_MOUSEWHEEL:
				return PlatformWindow::WindowMessages::MouseWheel;
			case WM_MOUSELEAVE:
				return PlatformWindow::WindowMessages::MouseLeave;
			case WM_MOUSEMOVE:
				return PlatformWindow::WindowMessages::MouseMove;
			}

			return PlatformWindow::WindowMessages::None;
		}

		PlatformWindow::VirtualKeys GetVirtualKey(uint32 Key)
		{
			switch (Key)
			{
			case VK_LBUTTON:
				return PlatformWindow::VirtualKeys::LeftButton;
			case VK_RBUTTON:
				return PlatformWindow::VirtualKeys::RightButton;
			case VK_MBUTTON:
				return PlatformWindow::VirtualKeys::MiddleButton;
			case VK_XBUTTON1:
				return PlatformWindow::VirtualKeys::X1Button;
			case VK_XBUTTON2:
				return PlatformWindow::VirtualKeys::X2Button;
			case VK_BACK:
				return PlatformWindow::VirtualKeys::Backspace;
			case VK_TAB:
				return PlatformWindow::VirtualKeys::Tab;
			case VK_RETURN:
				return PlatformWindow::VirtualKeys::Return;
			case VK_PAUSE:
				return PlatformWindow::VirtualKeys::Pause;
			case VK_ESCAPE:
				return PlatformWindow::VirtualKeys::Escape;
			case VK_SPACE:
				return PlatformWindow::VirtualKeys::Space;
			case VK_PRIOR:
				return PlatformWindow::VirtualKeys::PageUp;
			case VK_NEXT:
				return PlatformWindow::VirtualKeys::PageDown;
			case VK_END:
				return PlatformWindow::VirtualKeys::End;
			case VK_HOME:
				return PlatformWindow::VirtualKeys::Home;
			case VK_UP:
				return PlatformWindow::VirtualKeys::Up;
			case VK_DOWN:
				return PlatformWindow::VirtualKeys::Down;
			case VK_LEFT:
				return PlatformWindow::VirtualKeys::Left;
			case VK_RIGHT:
				return PlatformWindow::VirtualKeys::Right;
			case VK_SNAPSHOT:
				return PlatformWindow::VirtualKeys::PrintScreen;
			case VK_INSERT:
				return PlatformWindow::VirtualKeys::Insert;
			case VK_DELETE:
				return PlatformWindow::VirtualKeys::Delete;
			case VK_APPS:
				return PlatformWindow::VirtualKeys::ContextMenu;
			case VK_CAPITAL:
				return PlatformWindow::VirtualKeys::Capital;
			case VK_NUMLOCK:
				return PlatformWindow::VirtualKeys::NumLock;
			case VK_SCROLL:
				return PlatformWindow::VirtualKeys::ScrollLock;
			case VK_SHIFT:
				return PlatformWindow::VirtualKeys::Shift;
			case VK_LSHIFT:
				return PlatformWindow::VirtualKeys::LeftShift;
			case VK_RSHIFT:
				return PlatformWindow::VirtualKeys::RightShift;
			case VK_CONTROL:
				return PlatformWindow::VirtualKeys::Control;
			case VK_LCONTROL:
				return PlatformWindow::VirtualKeys::LeftControl;
			case VK_RCONTROL:
				return PlatformWindow::VirtualKeys::RightControl;
			case VK_MENU:
				return PlatformWindow::VirtualKeys::Alt;
			case VK_LMENU:
				return PlatformWindow::VirtualKeys::LeftAlt;
			case VK_RMENU:
				return PlatformWindow::VirtualKeys::RightAlt;
			case VK_LWIN:
				return PlatformWindow::VirtualKeys::LeftWindows;
			case VK_RWIN:
				return PlatformWindow::VirtualKeys::RightWindows;
			case VK_OEM_3:
				return PlatformWindow::VirtualKeys::KeypadApostrophe;
			case VK_OEM_MINUS:
				return PlatformWindow::VirtualKeys::KeypadMinus;
			case VK_OEM_PLUS:
				return PlatformWindow::VirtualKeys::KeypadPlus;
			case VK_OEM_4:
				return PlatformWindow::VirtualKeys::KeypadLeftSquareBraket;
			case VK_OEM_6:
				return PlatformWindow::VirtualKeys::KeypadRightSquareBraket;
			case VK_OEM_1:
				return PlatformWindow::VirtualKeys::KeypadColon;
			case VK_OEM_7:
				return PlatformWindow::VirtualKeys::KeypadQuotation;
			case VK_OEM_5:
				return PlatformWindow::VirtualKeys::KeypadBackSlash;
			case VK_OEM_COMMA:
				return PlatformWindow::VirtualKeys::KeypadComma;
			case VK_OEM_PERIOD:
				return PlatformWindow::VirtualKeys::KeypadDot;
			case VK_OEM_2:
				return PlatformWindow::VirtualKeys::KeypadForwardSlash;
			case '0':
				return PlatformWindow::VirtualKeys::Keypad0;
			case '1':
				return PlatformWindow::VirtualKeys::Keypad1;
			case '2':
				return PlatformWindow::VirtualKeys::Keypad2;
			case '3':
				return PlatformWindow::VirtualKeys::Keypad3;
			case '4':
				return PlatformWindow::VirtualKeys::Keypad4;
			case '5':
				return PlatformWindow::VirtualKeys::Keypad5;
			case '6':
				return PlatformWindow::VirtualKeys::Keypad6;
			case '7':
				return PlatformWindow::VirtualKeys::Keypad7;
			case '8':
				return PlatformWindow::VirtualKeys::Keypad8;
			case '9':
				return PlatformWindow::VirtualKeys::Keypad9;
			case 'A':
				return PlatformWindow::VirtualKeys::KeypadA;
			case 'B':
				return PlatformWindow::VirtualKeys::KeypadB;
			case 'C':
				return PlatformWindow::VirtualKeys::KeypadC;
			case 'D':
				return PlatformWindow::VirtualKeys::KeypadD;
			case 'E':
				return PlatformWindow::VirtualKeys::KeypadE;
			case 'F':
				return PlatformWindow::VirtualKeys::KeypadF;
			case 'G':
				return PlatformWindow::VirtualKeys::KeypadG;
			case 'H':
				return PlatformWindow::VirtualKeys::KeypadH;
			case 'I':
				return PlatformWindow::VirtualKeys::KeypadI;
			case 'J':
				return PlatformWindow::VirtualKeys::KeypadJ;
			case 'K':
				return PlatformWindow::VirtualKeys::KeypadK;
			case 'L':
				return PlatformWindow::VirtualKeys::KeypadL;
			case 'M':
				return PlatformWindow::VirtualKeys::KeypadM;
			case 'N':
				return PlatformWindow::VirtualKeys::KeypadN;
			case 'O':
				return PlatformWindow::VirtualKeys::KeypadO;
			case 'P':
				return PlatformWindow::VirtualKeys::KeypadP;
			case 'Q':
				return PlatformWindow::VirtualKeys::KeypadQ;
			case 'R':
				return PlatformWindow::VirtualKeys::KeypadR;
			case 'S':
				return PlatformWindow::VirtualKeys::KeypadS;
			case 'T':
				return PlatformWindow::VirtualKeys::KeypadT;
			case 'U':
				return PlatformWindow::VirtualKeys::KeypadU;
			case 'V':
				return PlatformWindow::VirtualKeys::KeypadV;
			case 'W':
				return PlatformWindow::VirtualKeys::KeypadW;
			case 'X':
				return PlatformWindow::VirtualKeys::KeypadX;
			case 'Y':
				return PlatformWindow::VirtualKeys::KeypadY;
			case 'Z':
				return PlatformWindow::VirtualKeys::KeypadZ;
			case VK_NUMPAD0:
				return PlatformWindow::VirtualKeys::Numpad0;
			case VK_NUMPAD1:
				return PlatformWindow::VirtualKeys::Numpad1;
			case VK_NUMPAD2:
				return PlatformWindow::VirtualKeys::Numpad2;
			case VK_NUMPAD3:
				return PlatformWindow::VirtualKeys::Numpad3;
			case VK_NUMPAD4:
				return PlatformWindow::VirtualKeys::Numpad4;
			case VK_NUMPAD5:
				return PlatformWindow::VirtualKeys::Numpad5;
			case VK_NUMPAD6:
				return PlatformWindow::VirtualKeys::Numpad6;
			case VK_NUMPAD7:
				return PlatformWindow::VirtualKeys::Numpad7;
			case VK_NUMPAD8:
				return PlatformWindow::VirtualKeys::Numpad8;
			case VK_NUMPAD9:
				return PlatformWindow::VirtualKeys::Numpad9;
			case VK_ADD:
				return PlatformWindow::VirtualKeys::NumpadAdd;
			case VK_SUBTRACT:
				return PlatformWindow::VirtualKeys::NumpadSubtract;
			case VK_MULTIPLY:
				return PlatformWindow::VirtualKeys::NumpadMultiply;
			case VK_DIVIDE:
				return PlatformWindow::VirtualKeys::NumpadDivide;
			case VK_DECIMAL:
				return PlatformWindow::VirtualKeys::NumpadDecimal;
			case VK_F1:
				return PlatformWindow::VirtualKeys::F1;
			case VK_F2:
				return PlatformWindow::VirtualKeys::F2;
			case VK_F3:
				return PlatformWindow::VirtualKeys::F3;
			case VK_F4:
				return PlatformWindow::VirtualKeys::F4;
			case VK_F5:
				return PlatformWindow::VirtualKeys::F5;
			case VK_F6:
				return PlatformWindow::VirtualKeys::F6;
			case VK_F7:
				return PlatformWindow::VirtualKeys::F7;
			case VK_F8:
				return PlatformWindow::VirtualKeys::F8;
			case VK_F9:
				return PlatformWindow::VirtualKeys::F9;
			case VK_F10:
				return PlatformWindow::VirtualKeys::F10;
			case VK_F11:
				return PlatformWindow::VirtualKeys::F11;
			case VK_F12:
				return PlatformWindow::VirtualKeys::F12;
			case VK_GAMEPAD_A:
				return PlatformWindow::VirtualKeys::GamepadA;
			case VK_GAMEPAD_B:
				return PlatformWindow::VirtualKeys::GamepadB;
			case VK_GAMEPAD_X:
				return PlatformWindow::VirtualKeys::GamepadX;
			case VK_GAMEPAD_Y:
				return PlatformWindow::VirtualKeys::GamepadY;
			case VK_GAMEPAD_RIGHT_SHOULDER:
				return PlatformWindow::VirtualKeys::GamepadRightShoulder;
			case VK_GAMEPAD_LEFT_SHOULDER:
				return PlatformWindow::VirtualKeys::GamepadLeftShoulder;
			case VK_GAMEPAD_LEFT_TRIGGER:
				return PlatformWindow::VirtualKeys::GamepadLeftTrigger;
			case VK_GAMEPAD_RIGHT_TRIGGER:
				return PlatformWindow::VirtualKeys::GamepadRightTrigger;
			case VK_GAMEPAD_DPAD_UP:
				return PlatformWindow::VirtualKeys::GamepadDPadUp;
			case VK_GAMEPAD_DPAD_DOWN:
				return PlatformWindow::VirtualKeys::GamepadDPadDown;
			case VK_GAMEPAD_DPAD_LEFT:
				return PlatformWindow::VirtualKeys::GamepadDPadLeft;
			case VK_GAMEPAD_DPAD_RIGHT:
				return PlatformWindow::VirtualKeys::GamepadDPadRight;
			case VK_GAMEPAD_MENU:
				return PlatformWindow::VirtualKeys::GamepadMenu;
			case VK_GAMEPAD_VIEW:
				return PlatformWindow::VirtualKeys::GamepadView;
			case VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON:
				return PlatformWindow::VirtualKeys::GamepadLeftThumbstickButton;
			case VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON:
				return PlatformWindow::VirtualKeys::GamepadRightThumbstickButton;
			case VK_GAMEPAD_LEFT_THUMBSTICK_UP:
				return PlatformWindow::VirtualKeys::GamepadLeftThumbstickUp;
			case VK_GAMEPAD_LEFT_THUMBSTICK_DOWN:
				return PlatformWindow::VirtualKeys::GamepadLeftThumbstickDowns;
			case VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT:
				return PlatformWindow::VirtualKeys::GamepadLeftThumbstickRight;
			case VK_GAMEPAD_LEFT_THUMBSTICK_LEFT:
				return PlatformWindow::VirtualKeys::GamepadLeftThumbstickLeft;
			case VK_GAMEPAD_RIGHT_THUMBSTICK_UP:
				return PlatformWindow::VirtualKeys::GamepadRightThumbstickUp;
			case VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN:
				return PlatformWindow::VirtualKeys::GamepadRightThumbstickDown;
			case VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT:
				return PlatformWindow::VirtualKeys::GamepadRightThumbstickRight;
			case VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT:
				return PlatformWindow::VirtualKeys::GamepadRightThumbstickLeft;
			}

			return PlatformWindow::VirtualKeys::COUNT;
		}

		int32 GetVirtualKey(PlatformWindow::VirtualKeys Key)
		{
			switch (Key)
			{
			case PlatformWindow::VirtualKeys::LeftButton:
				return VK_LBUTTON;
			case PlatformWindow::VirtualKeys::RightButton:
				return VK_RBUTTON;
			case PlatformWindow::VirtualKeys::MiddleButton:
				return VK_MBUTTON;
			case PlatformWindow::VirtualKeys::X1Button:
				return VK_XBUTTON1;
			case PlatformWindow::VirtualKeys::X2Button:
				return VK_XBUTTON2;
			case PlatformWindow::VirtualKeys::Backspace:
				return VK_BACK;
			case PlatformWindow::VirtualKeys::Tab:
				return VK_TAB;
			case PlatformWindow::VirtualKeys::Return:
				return VK_RETURN;
			case PlatformWindow::VirtualKeys::Pause:
				return VK_PAUSE;
			case PlatformWindow::VirtualKeys::Escape:
				return VK_ESCAPE;
			case PlatformWindow::VirtualKeys::Space:
				return VK_SPACE;
			case PlatformWindow::VirtualKeys::PageUp:
				return VK_PRIOR;
			case PlatformWindow::VirtualKeys::PageDown:
				return VK_NEXT;
			case PlatformWindow::VirtualKeys::End:
				return VK_END;
			case PlatformWindow::VirtualKeys::Home:
				return VK_HOME;
			case PlatformWindow::VirtualKeys::Up:
				return VK_UP;
			case PlatformWindow::VirtualKeys::Down:
				return VK_DOWN;
			case PlatformWindow::VirtualKeys::Left:
				return VK_LEFT;
			case PlatformWindow::VirtualKeys::Right:
				return VK_RIGHT;
			case PlatformWindow::VirtualKeys::PrintScreen:
				return VK_SNAPSHOT;
			case PlatformWindow::VirtualKeys::Insert:
				return VK_INSERT;
			case PlatformWindow::VirtualKeys::Delete:
				return VK_DELETE;
			case PlatformWindow::VirtualKeys::ContextMenu:
				return VK_APPS;
			case PlatformWindow::VirtualKeys::Capital:
				return VK_CAPITAL;
			case PlatformWindow::VirtualKeys::NumLock:
				return VK_NUMLOCK;
			case PlatformWindow::VirtualKeys::ScrollLock:
				return VK_SCROLL;
			case PlatformWindow::VirtualKeys::Shift:
				return VK_SHIFT;
			case PlatformWindow::VirtualKeys::LeftShift:
				return VK_LSHIFT;
			case PlatformWindow::VirtualKeys::RightShift:
				return VK_RSHIFT;
			case PlatformWindow::VirtualKeys::Control:
				return VK_CONTROL;
			case PlatformWindow::VirtualKeys::LeftControl:
				return VK_LCONTROL;
			case PlatformWindow::VirtualKeys::RightControl:
				return VK_RCONTROL;
			case PlatformWindow::VirtualKeys::Alt:
				return VK_MENU;
			case PlatformWindow::VirtualKeys::LeftAlt:
				return VK_LMENU;
			case PlatformWindow::VirtualKeys::RightAlt:
				return VK_RMENU;
			case PlatformWindow::VirtualKeys::LeftWindows:
				return VK_LWIN;
			case PlatformWindow::VirtualKeys::RightWindows:
				return VK_RWIN;
			case PlatformWindow::VirtualKeys::KeypadApostrophe:
				return VK_OEM_3;
			case PlatformWindow::VirtualKeys::KeypadMinus:
				return VK_OEM_MINUS;
			case PlatformWindow::VirtualKeys::KeypadPlus:
				return VK_OEM_PLUS;
			case PlatformWindow::VirtualKeys::KeypadLeftSquareBraket:
				return VK_OEM_4;
			case PlatformWindow::VirtualKeys::KeypadRightSquareBraket:
				return VK_OEM_6;
			case PlatformWindow::VirtualKeys::KeypadColon:
				return VK_OEM_1;
			case PlatformWindow::VirtualKeys::KeypadQuotation:
				return VK_OEM_7;
			case PlatformWindow::VirtualKeys::KeypadBackSlash:
				return VK_OEM_5;
			case PlatformWindow::VirtualKeys::KeypadComma:
				return VK_OEM_COMMA;
			case PlatformWindow::VirtualKeys::KeypadDot:
				return VK_OEM_PERIOD;
			case PlatformWindow::VirtualKeys::KeypadForwardSlash:
				return VK_OEM_2;
			case PlatformWindow::VirtualKeys::Keypad0:
				return '0';
			case PlatformWindow::VirtualKeys::Keypad1:
				return '1';
			case PlatformWindow::VirtualKeys::Keypad2:
				return '2';
			case PlatformWindow::VirtualKeys::Keypad3:
				return '3';
			case PlatformWindow::VirtualKeys::Keypad4:
				return '4';
			case PlatformWindow::VirtualKeys::Keypad5:
				return '5';
			case PlatformWindow::VirtualKeys::Keypad6:
				return '6';
			case PlatformWindow::VirtualKeys::Keypad7:
				return '7';
			case PlatformWindow::VirtualKeys::Keypad8:
				return '8';
			case PlatformWindow::VirtualKeys::Keypad9:
				return '9';
			case PlatformWindow::VirtualKeys::KeypadA:
				return 'A';
			case PlatformWindow::VirtualKeys::KeypadB:
				return 'B';
			case PlatformWindow::VirtualKeys::KeypadC:
				return 'C';
			case PlatformWindow::VirtualKeys::KeypadD:
				return 'D';
			case PlatformWindow::VirtualKeys::KeypadE:
				return 'E';
			case PlatformWindow::VirtualKeys::KeypadF:
				return 'F';
			case PlatformWindow::VirtualKeys::KeypadG:
				return 'G';
			case PlatformWindow::VirtualKeys::KeypadH:
				return 'H';
			case PlatformWindow::VirtualKeys::KeypadI:
				return 'I';
			case PlatformWindow::VirtualKeys::KeypadJ:
				return 'J';
			case PlatformWindow::VirtualKeys::KeypadK:
				return 'K';
			case PlatformWindow::VirtualKeys::KeypadL:
				return 'L';
			case PlatformWindow::VirtualKeys::KeypadM:
				return 'M';
			case PlatformWindow::VirtualKeys::KeypadN:
				return 'N';
			case PlatformWindow::VirtualKeys::KeypadO:
				return 'O';
			case PlatformWindow::VirtualKeys::KeypadP:
				return 'P';
			case PlatformWindow::VirtualKeys::KeypadQ:
				return 'Q';
			case PlatformWindow::VirtualKeys::KeypadR:
				return 'R';
			case PlatformWindow::VirtualKeys::KeypadS:
				return 'S';
			case PlatformWindow::VirtualKeys::KeypadT:
				return 'T';
			case PlatformWindow::VirtualKeys::KeypadU:
				return 'U';
			case PlatformWindow::VirtualKeys::KeypadV:
				return 'V';
			case PlatformWindow::VirtualKeys::KeypadW:
				return 'W';
			case PlatformWindow::VirtualKeys::KeypadX:
				return 'X';
			case PlatformWindow::VirtualKeys::KeypadY:
				return 'Y';
			case PlatformWindow::VirtualKeys::KeypadZ:
				return 'Z';
			case PlatformWindow::VirtualKeys::Numpad0:
				return VK_NUMPAD0;
			case PlatformWindow::VirtualKeys::Numpad1:
				return VK_NUMPAD1;
			case PlatformWindow::VirtualKeys::Numpad2:
				return VK_NUMPAD2;
			case PlatformWindow::VirtualKeys::Numpad3:
				return VK_NUMPAD3;
			case PlatformWindow::VirtualKeys::Numpad4:
				return VK_NUMPAD4;
			case PlatformWindow::VirtualKeys::Numpad5:
				return VK_NUMPAD5;
			case PlatformWindow::VirtualKeys::Numpad6:
				return VK_NUMPAD6;
			case PlatformWindow::VirtualKeys::Numpad7:
				return VK_NUMPAD7;
			case PlatformWindow::VirtualKeys::Numpad8:
				return VK_NUMPAD8;
			case PlatformWindow::VirtualKeys::Numpad9:
				return VK_NUMPAD9;
			case PlatformWindow::VirtualKeys::NumpadAdd:
				return VK_ADD;
			case PlatformWindow::VirtualKeys::NumpadSubtract:
				return VK_SUBTRACT;
			case PlatformWindow::VirtualKeys::NumpadMultiply:
				return VK_MULTIPLY;
			case PlatformWindow::VirtualKeys::NumpadDivide:
				return VK_DIVIDE;
			case PlatformWindow::VirtualKeys::NumpadDecimal:
				return VK_DECIMAL;
			case PlatformWindow::VirtualKeys::F1:
				return VK_F1;
			case PlatformWindow::VirtualKeys::F2:
				return VK_F2;
			case PlatformWindow::VirtualKeys::F3:
				return VK_F3;
			case PlatformWindow::VirtualKeys::F4:
				return VK_F4;
			case PlatformWindow::VirtualKeys::F5:
				return VK_F5;
			case PlatformWindow::VirtualKeys::F6:
				return VK_F6;
			case PlatformWindow::VirtualKeys::F7:
				return VK_F7;
			case PlatformWindow::VirtualKeys::F8:
				return VK_F8;
			case PlatformWindow::VirtualKeys::F9:
				return VK_F9;
			case PlatformWindow::VirtualKeys::F10:
				return VK_F10;
			case PlatformWindow::VirtualKeys::F11:
				return VK_F11;
			case PlatformWindow::VirtualKeys::F12:
				return VK_F12;
			case PlatformWindow::VirtualKeys::GamepadA:
				return VK_GAMEPAD_A;
			case PlatformWindow::VirtualKeys::GamepadB:
				return VK_GAMEPAD_B;
			case PlatformWindow::VirtualKeys::GamepadX:
				return VK_GAMEPAD_X;
			case PlatformWindow::VirtualKeys::GamepadY:
				return VK_GAMEPAD_Y;
			case PlatformWindow::VirtualKeys::GamepadRightShoulder:
				return VK_GAMEPAD_RIGHT_SHOULDER;
			case PlatformWindow::VirtualKeys::GamepadLeftShoulder:
				return VK_GAMEPAD_LEFT_SHOULDER;
			case PlatformWindow::VirtualKeys::GamepadLeftTrigger:
				return VK_GAMEPAD_LEFT_TRIGGER;
			case PlatformWindow::VirtualKeys::GamepadRightTrigger:
				return VK_GAMEPAD_RIGHT_TRIGGER;
			case PlatformWindow::VirtualKeys::GamepadDPadUp:
				return VK_GAMEPAD_DPAD_UP;
			case PlatformWindow::VirtualKeys::GamepadDPadDown:
				return VK_GAMEPAD_DPAD_DOWN;
			case PlatformWindow::VirtualKeys::GamepadDPadLeft:
				return VK_GAMEPAD_DPAD_LEFT;
			case PlatformWindow::VirtualKeys::GamepadDPadRight:
				return VK_GAMEPAD_DPAD_RIGHT;
			case PlatformWindow::VirtualKeys::GamepadMenu:
				return VK_GAMEPAD_MENU;
			case PlatformWindow::VirtualKeys::GamepadView:
				return VK_GAMEPAD_VIEW;
			case PlatformWindow::VirtualKeys::GamepadLeftThumbstickButton:
				return VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON;
			case PlatformWindow::VirtualKeys::GamepadRightThumbstickButton:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON;
			case PlatformWindow::VirtualKeys::GamepadLeftThumbstickUp:
				return VK_GAMEPAD_LEFT_THUMBSTICK_UP;
			case PlatformWindow::VirtualKeys::GamepadLeftThumbstickDowns:
				return VK_GAMEPAD_LEFT_THUMBSTICK_DOWN;
			case PlatformWindow::VirtualKeys::GamepadLeftThumbstickRight:
				return VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT;
			case PlatformWindow::VirtualKeys::GamepadLeftThumbstickLeft:
				return VK_GAMEPAD_LEFT_THUMBSTICK_LEFT;
			case PlatformWindow::VirtualKeys::GamepadRightThumbstickUp:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_UP;
			case PlatformWindow::VirtualKeys::GamepadRightThumbstickDown:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN;
			case PlatformWindow::VirtualKeys::GamepadRightThumbstickRight:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT;
			case PlatformWindow::VirtualKeys::GamepadRightThumbstickLeft:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT;
			}

			return 0;
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
					else if (Message == WM_KEYDOWN)
					{
						PlatformWindow::KeyInfo info;
						info.Key = GetVirtualKey(WParam);
						info.RepeatCount = LParam & 0xFFFF;

						result = pThis->m_Procedure(message, &info);
					}
					else if (Message == WM_KEYUP)
					{
						PlatformWindow::KeyInfo info;
						info.Key = GetVirtualKey(WParam);
						info.RepeatCount = 1;

						result = pThis->m_Procedure(message, &info);
					}
					else if (Message == WM_LBUTTONDOWN || Message == WM_LBUTTONUP)
					{
						PlatformWindow::MouseInfo info;
						info.Key = PlatformWindow::VirtualKeys::LeftButton;
						info.X = GET_X_LPARAM(LParam);
						info.Y = GET_Y_LPARAM(LParam);
						info.WheelDelta = 0;

						result = pThis->m_Procedure(message, &info);
					}
					else if (Message == WM_RBUTTONDOWN || Message == WM_RBUTTONUP)
					{
						PlatformWindow::MouseInfo info;
						info.Key = PlatformWindow::VirtualKeys::RightButton;
						info.X = GET_X_LPARAM(LParam);
						info.Y = GET_Y_LPARAM(LParam);
						info.WheelDelta = 0;

						result = pThis->m_Procedure(message, &info);
					}
					else if (Message == WM_MBUTTONDOWN || Message == WM_MBUTTONUP)
					{
						PlatformWindow::MouseInfo info;
						info.Key = PlatformWindow::VirtualKeys::MiddleButton;
						info.X = GET_X_LPARAM(LParam);
						info.Y = GET_Y_LPARAM(LParam);
						info.WheelDelta = 0;

						result = pThis->m_Procedure(message, &info);
					}
					else if (Message == WM_MOUSEWHEEL)
					{
						PlatformWindow::MouseInfo info;
						info.Key = PlatformWindow::VirtualKeys::COUNT;
						info.X = GET_X_LPARAM(LParam);
						info.Y = GET_Y_LPARAM(LParam);
						info.WheelDelta = GET_WHEEL_DELTA_WPARAM(WParam); //WHEEL_DELTA

						result = pThis->m_Procedure(message, &info);
					}
					else if (Message == WM_MOUSEMOVE)
					{
						PlatformWindow::MouseInfo info;
						info.Key = PlatformWindow::VirtualKeys::COUNT;
						info.X = GET_X_LPARAM(LParam);
						info.Y = GET_Y_LPARAM(LParam);
						info.WheelDelta = 0;

						result = pThis->m_Procedure(message, &info);
					}
					else if (Message == WM_MOUSELEAVE)
					{
						result = pThis->m_Procedure(message, nullptr);
					}
					else
						result = pThis->m_Procedure(message, nullptr);
				}

				return DefWindowProc((HWND)Handle, (UINT)Message, (WPARAM)WParam, (LPARAM)LParam);
			}

		private:
			PlatformWindow::Procedure m_Procedure;
		};

		//TODO: change all bitwise if to SET_IF_ENABLED style, Rendering, File, looking for |= and &=
		DWORD GetStyleMask(PlatformWindow::Styles Style)
		{
			DWORD style = 0;

			SET_IF_ENABLED(Style, PlatformWindow::Styles::Popup, style, WS_POPUP);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::Visible, style, WS_VISIBLE);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::Caption, style, WS_CAPTION);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::Disabled, style, WS_DISABLED);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::ClipSiblings, style, WS_CLIPSIBLINGS);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::ClipChildren, style, WS_CLIPCHILDREN);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::Minimize, style, WS_MINIMIZE);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::Maximize, style, WS_MAXIMIZE);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::Border, style, WS_BORDER);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::DialogFrame, style, WS_DLGFRAME);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::VerticalScroll, style, WS_VSCROLL);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::HorizontalScroll, style, WS_HSCROLL);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::SystemMenu, style, WS_SYSMENU);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::ThickFrame, style, WS_THICKFRAME);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::Group, style, WS_GROUP);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::TabStop, style, WS_TABSTOP);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::MinimizeBox, style, WS_MINIMIZEBOX);
			SET_IF_ENABLED(Style, PlatformWindow::Styles::MaximizeBox, style, WS_MAXIMIZEBOX);

			return style;
		}

		DWORD GetExtraStyleMax(PlatformWindow::ExtraStyles Style)
		{
			DWORD style = 0;

			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::DialogModalFrame, style, WS_EX_DLGMODALFRAME);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::NoParentNotify, style, WS_EX_NOPARENTNOTIFY);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::TopMost, style, WS_EX_TOPMOST);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::AcceptFiles, style, WS_EX_ACCEPTFILES);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::Transparent, style, WS_EX_TRANSPARENT);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::MDIChild, style, WS_EX_MDICHILD);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::ToolWindow, style, WS_EX_TOOLWINDOW);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::WindowEdge, style, WS_EX_WINDOWEDGE);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::ClientEdge, style, WS_EX_CLIENTEDGE);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::ContextHelp, style, WS_EX_CONTEXTHELP);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::Right, style, WS_EX_RIGHT);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::Left, style, WS_EX_LEFT);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::RTLReading, style, WS_EX_RTLREADING);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::LTRReading, style, WS_EX_LTRREADING);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::LeftScrollBar, style, WS_EX_LEFTSCROLLBAR);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::RightScrollBar, style, WS_EX_RIGHTSCROLLBAR);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::ControlParent, style, WS_EX_CONTROLPARENT);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::StaticEdge, style, WS_EX_STATICEDGE);
			SET_IF_ENABLED(Style, PlatformWindow::ExtraStyles::AppWindow, style, WS_EX_APPWINDOW);

			return style;
		}

		DWORD GetShowWindowState(PlatformWindow::ShowWindowStates State)
		{
			switch (State)
			{
			case PlatformWindow::ShowWindowStates::Restore:
				return SW_RESTORE;
			case PlatformWindow::ShowWindowStates::Hide:
				return SW_HIDE;
			case PlatformWindow::ShowWindowStates::Show:
				return SW_SHOW;
			case PlatformWindow::ShowWindowStates::ShowInactive:
				return SW_SHOWNA;
			case PlatformWindow::ShowWindowStates::ShowDefault:
				return SW_SHOWDEFAULT;
			case PlatformWindow::ShowWindowStates::ShowMinimized:
				return SW_SHOWMINIMIZED;
			case PlatformWindow::ShowWindowStates::ShowMaximized:
				return SW_SHOWMAXIMIZED;
			case PlatformWindow::ShowWindowStates::ShowInactiveMinimized:
				return SW_SHOWMINNOACTIVE;
			case PlatformWindow::ShowWindowStates::ShowNormal:
				return SW_SHOWNORMAL;
			case PlatformWindow::ShowWindowStates::ShowInactiveNormal:
				return SW_SHOWNOACTIVATE;
			case PlatformWindow::ShowWindowStates::Minimize:
				return SW_MINIMIZE;
			case PlatformWindow::ShowWindowStates::Maximize:
				return SW_MAXIMIZE;
			}

			return SW_NORMAL;
		}

		HWND GetPlace(PlatformWindow::Places State)
		{
			switch (State)
			{
			case PlatformWindow::Places::Top:
				return HWND_TOP;
			case PlatformWindow::Places::Bottom:
				return HWND_BOTTOM;
			case PlatformWindow::Places::TopMost:
				return HWND_TOPMOST;
			case PlatformWindow::Places::NoTopMost:
				return HWND_NOTOPMOST;
			}

			return HWND_TOP;
		}

		BYTE GetPixelType(PlatformWindow::PixelTypes Type)
		{
			BYTE style = 0;

			SET_IF_ENABLED(Type, PlatformWindow::PixelTypes::RGBA, style, PFD_TYPE_RGBA);
			SET_IF_ENABLED(Type, PlatformWindow::PixelTypes::ColorIndex, style, PFD_TYPE_COLORINDEX);

			return style;
		}

		BYTE GetLayerType(PlatformWindow::LayerTypes Type)
		{
			BYTE style = 0;

			SET_IF_ENABLED(Type, PlatformWindow::LayerTypes::MainPlane, style, PFD_MAIN_PLANE);
			SET_IF_ENABLED(Type, PlatformWindow::LayerTypes::OverlayPlane, style, PFD_OVERLAY_PLANE);
			SET_IF_ENABLED(Type, PlatformWindow::LayerTypes::UnderlayPlane, style, PFD_UNDERLAY_PLANE);

			return style;
		}

		DWORD GetPixelFormat(PlatformWindow::PixelFormats Type)
		{
			DWORD style = 0;

			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::DoubleBuffer, style, PFD_DOUBLEBUFFER);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::Stereo, style, PFD_STEREO);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::DrawToWindow, style, PFD_DRAW_TO_WINDOW);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::DrawToBitmap, style, PFD_DRAW_TO_BITMAP);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::SupportGDI, style, PFD_SUPPORT_GDI);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::SupportOpenGL, style, PFD_SUPPORT_OPENGL);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::GenericFormat, style, PFD_GENERIC_FORMAT);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::NeedPalette, style, PFD_NEED_PALETTE);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::NeedSystemPalette, style, PFD_NEED_SYSTEM_PALETTE);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::SwapExchange, style, PFD_SWAP_EXCHANGE);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::SwapCopy, style, PFD_SWAP_COPY);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::SwapLayerBuffers, style, PFD_SWAP_LAYER_BUFFERS);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::GenericAccelerated, style, PFD_GENERIC_ACCELERATED);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::SupportDirectDraw, style, PFD_SUPPORT_DIRECTDRAW);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::Direct3DAccelerated, style, PFD_DIRECT3D_ACCELERATED);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::SupportComposition, style, PFD_SUPPORT_COMPOSITION);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::DepthDontCare, style, PFD_DEPTH_DONTCARE);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::DoubleBufferDontCare, style, PFD_DOUBLEBUFFER_DONTCARE);
			SET_IF_ENABLED(Type, PlatformWindow::PixelFormats::StereoDontCare, style, PFD_STEREO_DONTCARE);

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

		void PlatformWindow::ShowWindow(WindowHandle Handle, ShowWindowStates State)
		{
			::ShowWindow((HWND)Handle, GetShowWindowState(State));
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

		void PlatformWindow::SetPositionOption(WindowHandle Handle, Places Place)
		{
			SetWindowPos((HWND)Handle, GetPlace(Place), 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOMOVE);
		}

		void PlatformWindow::SetPosition(WindowHandle Handle, uint16 X, uint16 Y)
		{
			SetWindowPos((HWND)Handle, nullptr, X, Y, 0, 0, SWP_NOSIZE);
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
			SetWindowPos((HWND)Handle, nullptr, 0, 0, Width, Height, SWP_NOMOVE);
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
		}

		PlatformWindow::Styles PlatformWindow::GetStyle(WindowHandle Handle)
		{
			DWORD wStyle = GetWindowLongPtr((HWND)Handle, GWL_STYLE);

			Styles style = (Styles)0;

			SET_IF_ENABLED(wStyle, WS_OVERLAPPED, style, PlatformWindow::Styles::Overlapped);
			SET_IF_ENABLED(wStyle, WS_POPUP, style, PlatformWindow::Styles::Popup);
			SET_IF_ENABLED(wStyle, WS_CHILD, style, PlatformWindow::Styles::Child);
			SET_IF_ENABLED(wStyle, WS_VISIBLE, style, PlatformWindow::Styles::Visible);
			SET_IF_ENABLED(wStyle, WS_DISABLED, style, PlatformWindow::Styles::Disabled);
			SET_IF_ENABLED(wStyle, WS_CLIPSIBLINGS, style, PlatformWindow::Styles::ClipSiblings);
			SET_IF_ENABLED(wStyle, WS_CLIPCHILDREN, style, PlatformWindow::Styles::ClipChildren);
			SET_IF_ENABLED(wStyle, WS_MINIMIZE, style, PlatformWindow::Styles::Minimize);
			SET_IF_ENABLED(wStyle, WS_MAXIMIZE, style, PlatformWindow::Styles::Maximize);
			SET_IF_ENABLED(wStyle, WS_BORDER, style, PlatformWindow::Styles::Border);
			SET_IF_ENABLED(wStyle, WS_DLGFRAME, style, PlatformWindow::Styles::DialogFrame);
			SET_IF_ENABLED(wStyle, WS_VSCROLL, style, PlatformWindow::Styles::VerticalScroll);
			SET_IF_ENABLED(wStyle, WS_HSCROLL, style, PlatformWindow::Styles::HorizontalScroll);
			SET_IF_ENABLED(wStyle, WS_SYSMENU, style, PlatformWindow::Styles::SystemMenu);
			SET_IF_ENABLED(wStyle, WS_THICKFRAME, style, PlatformWindow::Styles::ThickFrame);
			SET_IF_ENABLED(wStyle, WS_GROUP, style, PlatformWindow::Styles::Group);
			SET_IF_ENABLED(wStyle, WS_TABSTOP, style, PlatformWindow::Styles::TabStop);
			SET_IF_ENABLED(wStyle, WS_MINIMIZEBOX, style, PlatformWindow::Styles::MinimizeBox);
			SET_IF_ENABLED(wStyle, WS_MAXIMIZEBOX, style, PlatformWindow::Styles::MaximizeBox);

			return style;
		}

		void PlatformWindow::SetExtraStyle(WindowHandle Handle, ExtraStyles Style)
		{
			SetWindowLongPtr((HWND)Handle, GWL_EXSTYLE, GetExtraStyleMax(Style));
		}

		PlatformWindow::ExtraStyles PlatformWindow::GetExtraStyle(WindowHandle Handle)
		{
			DWORD wStyle = GetWindowLongPtr((HWND)Handle, GWL_EXSTYLE);

			ExtraStyles style = (ExtraStyles)0;

			SET_IF_ENABLED(wStyle, WS_EX_DLGMODALFRAME, style, PlatformWindow::ExtraStyles::DialogModalFrame);
			SET_IF_ENABLED(wStyle, WS_EX_TOPMOST, style, PlatformWindow::ExtraStyles::TopMost);
			SET_IF_ENABLED(wStyle, WS_EX_ACCEPTFILES, style, PlatformWindow::ExtraStyles::AcceptFiles);
			SET_IF_ENABLED(wStyle, WS_EX_TRANSPARENT, style, PlatformWindow::ExtraStyles::Transparent);
			SET_IF_ENABLED(wStyle, WS_EX_MDICHILD, style, PlatformWindow::ExtraStyles::MDIChild);
			SET_IF_ENABLED(wStyle, WS_EX_TOOLWINDOW, style, PlatformWindow::ExtraStyles::ToolWindow);
			SET_IF_ENABLED(wStyle, WS_EX_WINDOWEDGE, style, PlatformWindow::ExtraStyles::WindowEdge);
			SET_IF_ENABLED(wStyle, WS_EX_CLIENTEDGE, style, PlatformWindow::ExtraStyles::ClientEdge);
			SET_IF_ENABLED(wStyle, WS_EX_CONTEXTHELP, style, PlatformWindow::ExtraStyles::ContextHelp);
			SET_IF_ENABLED(wStyle, WS_EX_RIGHT, style, PlatformWindow::ExtraStyles::Right);
			SET_IF_ENABLED(wStyle, WS_EX_LEFT, style, PlatformWindow::ExtraStyles::Left);
			SET_IF_ENABLED(wStyle, WS_EX_RTLREADING, style, PlatformWindow::ExtraStyles::RTLReading);
			SET_IF_ENABLED(wStyle, WS_EX_LTRREADING, style, PlatformWindow::ExtraStyles::LTRReading);
			SET_IF_ENABLED(wStyle, WS_EX_LEFTSCROLLBAR, style, PlatformWindow::ExtraStyles::LeftScrollBar);
			SET_IF_ENABLED(wStyle, WS_EX_RIGHTSCROLLBAR, style, PlatformWindow::ExtraStyles::RightScrollBar);
			SET_IF_ENABLED(wStyle, WS_EX_CONTROLPARENT, style, PlatformWindow::ExtraStyles::ControlParent);
			SET_IF_ENABLED(wStyle, WS_EX_STATICEDGE, style, PlatformWindow::ExtraStyles::StaticEdge);
			SET_IF_ENABLED(wStyle, WS_EX_APPWINDOW, style, PlatformWindow::ExtraStyles::AppWindow);

			return style;
		}

		void PlatformWindow::Invalidate(WindowHandle Handle)
		{
			InvalidateRect((HWND)Handle, nullptr, false);
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

		void PlatformWindow::MakeCurrentWGLContext(ContextHandle ContextHandle, WGLContextHandle WGLContextHandle)
		{
			wglMakeCurrent((HDC)ContextHandle, (HGLRC)WGLContextHandle);
		}

		void PlatformWindow::GetCurrentWGLContext(ContextHandle & ContextHandle, WGLContextHandle &WGLContextHandle)
		{
			ContextHandle = (PlatformWindow::ContextHandle)wglGetCurrentDC();
			WGLContextHandle = (PlatformWindow::WGLContextHandle)wglGetCurrentContext();
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

		bool PlatformWindow::GetState(VirtualKeys Key)
		{
			return (GetKeyState(GetVirtualKey(Key)) & 0x8000);
		}

		void PlatformWindow::GetMousePosition(int32& X, int32& Y)
		{
			POINT point;
			GetCursorPos(&point);

			X = point.x;
			Y = point.y;
		}
	}
}
#endif