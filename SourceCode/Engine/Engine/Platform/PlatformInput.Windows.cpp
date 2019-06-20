// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformInput.h>
#include <Windows.h>

namespace Engine
{
	namespace Platform
	{
		//http://www.kbdedit.com/manual/low_level_vk_list.html

		//int32 GetVirtualKey(PlatformInput::VirtualKeys Key)
		//{
		//	switch (Key)
		//	{
		//	case PlatformInput::VirtualKeys::LeftButton:
		//		return VK_LBUTTON;
		//	case PlatformInput::VirtualKeys::RightButton:
		//		return VK_RBUTTON;
		//	case PlatformInput::VirtualKeys::MiddleButton:
		//		return VK_MBUTTON;
		//	case PlatformInput::VirtualKeys::X1Button:
		//		return VK_XBUTTON1;
		//	case PlatformInput::VirtualKeys::X2Button:
		//		return VK_XBUTTON2;
		//	case PlatformInput::VirtualKeys::Backspace:
		//		return VK_BACK;
		//	case PlatformInput::VirtualKeys::Tab:
		//		return VK_TAB;
		//	case PlatformInput::VirtualKeys::Return:
		//		return VK_RETURN;
		//	case PlatformInput::VirtualKeys::Shift:
		//		return VK_SHIFT;
		//	case PlatformInput::VirtualKeys::LeftShift:
		//		return VK_LSHIFT;
		//	case PlatformInput::VirtualKeys::RightShift:
		//		return VK_RSHIFT;
		//	case PlatformInput::VirtualKeys::Control:
		//		return VK_CONTROL;
		//	case PlatformInput::VirtualKeys::LeftControl:
		//		return VK_LCONTROL;
		//	case PlatformInput::VirtualKeys::RightControl:
		//		return VK_RCONTROL;
		//	case PlatformInput::VirtualKeys::Alt:
		//		return VK_MENU;
		//	case PlatformInput::VirtualKeys::LeftAlt:
		//		return VK_LMENU;
		//	case PlatformInput::VirtualKeys::RightAlt:
		//		return VK_RMENU;
		//	case PlatformInput::VirtualKeys::Pause:
		//		return VK_PAUSE;
		//	case PlatformInput::VirtualKeys::Capital:
		//		return VK_CAPITAL;
		//	case PlatformInput::VirtualKeys::Escape:
		//		return VK_ESCAPE;
		//	case PlatformInput::VirtualKeys::Space:
		//		return VK_SPACE;
		//	case PlatformInput::VirtualKeys::PageUp:
		//		return VK_PRIOR;
		//	case PlatformInput::VirtualKeys::PageDown:
		//		return VK_NEXT;
		//	case PlatformInput::VirtualKeys::End:
		//		return VK_END;
		//	case PlatformInput::VirtualKeys::Home:
		//		return VK_HOME;
		//	case PlatformInput::VirtualKeys::Left:
		//		return VK_LEFT;
		//	case PlatformInput::VirtualKeys::Up:
		//		return VK_UP;
		//	case PlatformInput::VirtualKeys::Right:
		//		return VK_RIGHT;
		//	case PlatformInput::VirtualKeys::Down:
		//		return VK_DOWN;
		//	case PlatformInput::VirtualKeys::PrintScreen:
		//		return VK_SNAPSHOT;
		//	case PlatformInput::VirtualKeys::Insert:
		//		return VK_INSERT;
		//	case PlatformInput::VirtualKeys::Delete:
		//		return VK_DELETE;
		//	case PlatformInput::VirtualKeys::LeftWindows:
		//		return VK_LWIN;
		//	case PlatformInput::VirtualKeys::RightWindows:
		//		return VK_RWIN;
		//	case PlatformInput::VirtualKeys::ContextMenu:
		//		return VK_APPS;
		//	case PlatformInput::VirtualKeys::Numpad0:
		//		return VK_NUMPAD0;
		//	case PlatformInput::VirtualKeys::Numpad1:
		//		return VK_NUMPAD1;
		//	case PlatformInput::VirtualKeys::Numpad2:
		//		return VK_NUMPAD2;
		//	case PlatformInput::VirtualKeys::Numpad3:
		//		return VK_NUMPAD3;
		//	case PlatformInput::VirtualKeys::Numpad4:
		//		return VK_NUMPAD4;
		//	case PlatformInput::VirtualKeys::Numpad5:
		//		return VK_NUMPAD5;
		//	case PlatformInput::VirtualKeys::Numpad6:
		//		return VK_NUMPAD6;
		//	case PlatformInput::VirtualKeys::Numpad7:
		//		return VK_NUMPAD7;
		//	case PlatformInput::VirtualKeys::Numpad8:
		//		return VK_NUMPAD8;
		//	case PlatformInput::VirtualKeys::Numpad9:
		//		return VK_NUMPAD9;
		//	case PlatformInput::VirtualKeys::NumpadMultiply:
		//		return VK_MULTIPLY;
		//	case PlatformInput::VirtualKeys::NumpadAdd:
		//		return VK_ADD;
		//	case PlatformInput::VirtualKeys::NumpadSubtract:
		//		return VK_SUBTRACT;
		//	case PlatformInput::VirtualKeys::NumpadDecimal:
		//		return VK_DECIMAL;
		//	case PlatformInput::VirtualKeys::NumpadDivide:
		//		return VK_DIVIDE;
		//	case PlatformInput::VirtualKeys::F1:
		//		return VK_F1;
		//	case PlatformInput::VirtualKeys::F2:
		//		return VK_F2;
		//	case PlatformInput::VirtualKeys::F3:
		//		return VK_F3;
		//	case PlatformInput::VirtualKeys::F4:
		//		return VK_F4;
		//	case PlatformInput::VirtualKeys::F5:
		//		return VK_F5;
		//	case PlatformInput::VirtualKeys::F6:
		//		return VK_F6;
		//	case PlatformInput::VirtualKeys::F7:
		//		return VK_F7;
		//	case PlatformInput::VirtualKeys::F8:
		//		return VK_F8;
		//	case PlatformInput::VirtualKeys::F9:
		//		return VK_F9;
		//	case PlatformInput::VirtualKeys::F10:
		//		return VK_F10;
		//	case PlatformInput::VirtualKeys::F11:
		//		return VK_F11;
		//	case PlatformInput::VirtualKeys::F12:
		//		return VK_F12;
		//	case PlatformInput::VirtualKeys::NumLock:
		//		return VK_NUMLOCK;
		//	case PlatformInput::VirtualKeys::ScrollLock:
		//		return VK_SCROLL;
		//	case PlatformInput::VirtualKeys::GamepadA:
		//		return VK_GAMEPAD_A;
		//	case PlatformInput::VirtualKeys::GamepadB:
		//		return VK_GAMEPAD_B;
		//	case PlatformInput::VirtualKeys::GamepadX:
		//		return VK_GAMEPAD_X;
		//	case PlatformInput::VirtualKeys::GamepadY:
		//		return VK_GAMEPAD_Y;
		//	case PlatformInput::VirtualKeys::GamepadRightShoulder:
		//		return VK_GAMEPAD_RIGHT_SHOULDER;
		//	case PlatformInput::VirtualKeys::GamepadLeftShoulder:
		//		return VK_GAMEPAD_LEFT_SHOULDER;
		//	case PlatformInput::VirtualKeys::GamepadLeftTrigger:
		//		return VK_GAMEPAD_LEFT_TRIGGER;
		//	case PlatformInput::VirtualKeys::GamepadRightTrigger:
		//		return VK_GAMEPAD_RIGHT_TRIGGER;
		//	case PlatformInput::VirtualKeys::GamepadDPadUp:
		//		return VK_GAMEPAD_DPAD_UP;
		//	case PlatformInput::VirtualKeys::GamepadDPadDown:
		//		return VK_GAMEPAD_DPAD_DOWN;
		//	case PlatformInput::VirtualKeys::GamepadDPadLeft:
		//		return VK_GAMEPAD_DPAD_LEFT;
		//	case PlatformInput::VirtualKeys::GamepadDPadRight:
		//		return VK_GAMEPAD_DPAD_RIGHT;
		//	case PlatformInput::VirtualKeys::GamepadMenu:
		//		return VK_GAMEPAD_MENU;
		//	case PlatformInput::VirtualKeys::GamepadView:
		//		return VK_GAMEPAD_VIEW;
		//	case PlatformInput::VirtualKeys::GamepadLeftThumbstickButton:
		//		return VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON;
		//	case PlatformInput::VirtualKeys::GamepadRightThumbstickButton:
		//		return VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON;
		//	case PlatformInput::VirtualKeys::GamepadLeftThumbstickUp:
		//		return VK_GAMEPAD_LEFT_THUMBSTICK_UP;
		//	case PlatformInput::VirtualKeys::GamepadLeftThumbstickDowns:
		//		return VK_GAMEPAD_LEFT_THUMBSTICK_DOWN;
		//	case PlatformInput::VirtualKeys::GamepadLeftThumbstickRight:
		//		return VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT;
		//	case PlatformInput::VirtualKeys::GamepadLeftThumbstickLeft:
		//		return VK_GAMEPAD_LEFT_THUMBSTICK_LEFT;
		//	case PlatformInput::VirtualKeys::GamepadRightThumbstickUp:
		//		return VK_GAMEPAD_RIGHT_THUMBSTICK_UP;
		//	case PlatformInput::VirtualKeys::GamepadRightThumbstickDown:
		//		return VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN;
		//	case PlatformInput::VirtualKeys::GamepadRightThumbstickRight:
		//		return VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT;
		//	case PlatformInput::VirtualKeys::GamepadRightThumbstickLeft:
		//		return VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT;
		//	}

		//	return 0;
		//}

		int32 GetVirtualKey(PlatformInput::VirtualKeys Key)
		{
			switch (Key)
			{
			case PlatformInput::VirtualKeys::LeftButton:
				return VK_LBUTTON;
			case PlatformInput::VirtualKeys::RightButton:
				return VK_RBUTTON;
			case PlatformInput::VirtualKeys::MiddleButton:
				return VK_MBUTTON;
			case PlatformInput::VirtualKeys::X1Button:
				return VK_XBUTTON1;
			case PlatformInput::VirtualKeys::X2Button:
				return VK_XBUTTON2;
			case PlatformInput::VirtualKeys::Backspace:
				return VK_BACK;
			case PlatformInput::VirtualKeys::Tab:
				return VK_TAB;
			case PlatformInput::VirtualKeys::Return:
				return VK_RETURN;
			case PlatformInput::VirtualKeys::Pause:
				return VK_PAUSE;
			case PlatformInput::VirtualKeys::Escape:
				return VK_ESCAPE;
			case PlatformInput::VirtualKeys::Space:
				return VK_SPACE;
			case PlatformInput::VirtualKeys::PageUp:
				return VK_PRIOR;
			case PlatformInput::VirtualKeys::PageDown:
				return VK_NEXT;
			case PlatformInput::VirtualKeys::End:
				return VK_END;
			case PlatformInput::VirtualKeys::Home:
				return VK_HOME;
			case PlatformInput::VirtualKeys::Up:
				return VK_UP;
			case PlatformInput::VirtualKeys::Down:
				return VK_DOWN;
			case PlatformInput::VirtualKeys::Left:
				return VK_LEFT;
			case PlatformInput::VirtualKeys::Right:
				return VK_RIGHT;
			case PlatformInput::VirtualKeys::PrintScreen:
				return VK_SNAPSHOT;
			case PlatformInput::VirtualKeys::Insert:
				return VK_INSERT;
			case PlatformInput::VirtualKeys::Delete:
				return VK_DELETE;
			case PlatformInput::VirtualKeys::ContextMenu:
				return VK_APPS;
			case PlatformInput::VirtualKeys::Capital:
				return VK_CAPITAL;
			case PlatformInput::VirtualKeys::NumLock:
				return VK_NUMLOCK;
			case PlatformInput::VirtualKeys::ScrollLock:
				return VK_SCROLL;
			case PlatformInput::VirtualKeys::Shift:
				return VK_SHIFT;
			case PlatformInput::VirtualKeys::LeftShift:
				return VK_LSHIFT;
			case PlatformInput::VirtualKeys::RightShift:
				return VK_RSHIFT;
			case PlatformInput::VirtualKeys::Control:
				return VK_CONTROL;
			case PlatformInput::VirtualKeys::LeftControl:
				return VK_LCONTROL;
			case PlatformInput::VirtualKeys::RightControl:
				return VK_RCONTROL;
			case PlatformInput::VirtualKeys::Alt:
				return VK_MENU;
			case PlatformInput::VirtualKeys::LeftAlt:
				return VK_LMENU;
			case PlatformInput::VirtualKeys::RightAlt:
				return VK_RMENU;
			case PlatformInput::VirtualKeys::LeftWindows:
				return VK_LWIN;
			case PlatformInput::VirtualKeys::RightWindows:
				return VK_RWIN;
			case PlatformInput::VirtualKeys::KeypadApostrophe:
				return VK_OEM_3;
			case PlatformInput::VirtualKeys::KeypadMinus:
				return VK_OEM_MINUS;
			case PlatformInput::VirtualKeys::KeypadPlus:
				return VK_OEM_PLUS;
			case PlatformInput::VirtualKeys::KeypadLeftSquareBraket:
				return VK_OEM_4;
			case PlatformInput::VirtualKeys::KeypadRightSquareBraket:
				return VK_OEM_6;
			case PlatformInput::VirtualKeys::KeypadColon:
				return VK_OEM_1;
			case PlatformInput::VirtualKeys::KeypadQuotation:
				return VK_OEM_7;
			case PlatformInput::VirtualKeys::KeypadBackSlash:
				return VK_OEM_5;
			case PlatformInput::VirtualKeys::KeypadComma:
				return VK_OEM_COMMA;
			case PlatformInput::VirtualKeys::KeypadDot:
				return VK_OEM_PERIOD;
			case PlatformInput::VirtualKeys::KeypadForwardSlash:
				return VK_OEM_2;
			case PlatformInput::VirtualKeys::Keypad0:
				return '0';
			case PlatformInput::VirtualKeys::Keypad1:
				return '1';
			case PlatformInput::VirtualKeys::Keypad2:
				return '2';
			case PlatformInput::VirtualKeys::Keypad3:
				return '3';
			case PlatformInput::VirtualKeys::Keypad4:
				return '4';
			case PlatformInput::VirtualKeys::Keypad5:
				return '5';
			case PlatformInput::VirtualKeys::Keypad6:
				return '6';
			case PlatformInput::VirtualKeys::Keypad7:
				return '7';
			case PlatformInput::VirtualKeys::Keypad8:
				return '8';
			case PlatformInput::VirtualKeys::Keypad9:
				return '9';
			case PlatformInput::VirtualKeys::KeypadA:
				return 'A';
			case PlatformInput::VirtualKeys::KeypadB:
				return 'B';
			case PlatformInput::VirtualKeys::KeypadC:
				return 'C';
			case PlatformInput::VirtualKeys::KeypadD:
				return 'D';
			case PlatformInput::VirtualKeys::KeypadE:
				return 'E';
			case PlatformInput::VirtualKeys::KeypadF:
				return 'F';
			case PlatformInput::VirtualKeys::KeypadG:
				return 'G';
			case PlatformInput::VirtualKeys::KeypadH:
				return 'H';
			case PlatformInput::VirtualKeys::KeypadI:
				return 'I';
			case PlatformInput::VirtualKeys::KeypadJ:
				return 'J';
			case PlatformInput::VirtualKeys::KeypadK:
				return 'K';
			case PlatformInput::VirtualKeys::KeypadL:
				return 'L';
			case PlatformInput::VirtualKeys::KeypadM:
				return 'M';
			case PlatformInput::VirtualKeys::KeypadN:
				return 'N';
			case PlatformInput::VirtualKeys::KeypadO:
				return 'O';
			case PlatformInput::VirtualKeys::KeypadP:
				return 'P';
			case PlatformInput::VirtualKeys::KeypadQ:
				return 'Q';
			case PlatformInput::VirtualKeys::KeypadR:
				return 'R';
			case PlatformInput::VirtualKeys::KeypadS:
				return 'S';
			case PlatformInput::VirtualKeys::KeypadT:
				return 'T';
			case PlatformInput::VirtualKeys::KeypadU:
				return 'U';
			case PlatformInput::VirtualKeys::KeypadV:
				return 'V';
			case PlatformInput::VirtualKeys::KeypadW:
				return 'W';
			case PlatformInput::VirtualKeys::KeypadX:
				return 'X';
			case PlatformInput::VirtualKeys::KeypadY:
				return 'Y';
			case PlatformInput::VirtualKeys::KeypadZ:
				return 'Z';
			case PlatformInput::VirtualKeys::Numpad0:
				return VK_NUMPAD0;
			case PlatformInput::VirtualKeys::Numpad1:
				return VK_NUMPAD1;
			case PlatformInput::VirtualKeys::Numpad2:
				return VK_NUMPAD2;
			case PlatformInput::VirtualKeys::Numpad3:
				return VK_NUMPAD3;
			case PlatformInput::VirtualKeys::Numpad4:
				return VK_NUMPAD4;
			case PlatformInput::VirtualKeys::Numpad5:
				return VK_NUMPAD5;
			case PlatformInput::VirtualKeys::Numpad6:
				return VK_NUMPAD6;
			case PlatformInput::VirtualKeys::Numpad7:
				return VK_NUMPAD7;
			case PlatformInput::VirtualKeys::Numpad8:
				return VK_NUMPAD8;
			case PlatformInput::VirtualKeys::Numpad9:
				return VK_NUMPAD9;
			case PlatformInput::VirtualKeys::NumpadAdd:
				return VK_ADD;
			case PlatformInput::VirtualKeys::NumpadSubtract:
				return VK_SUBTRACT;
			case PlatformInput::VirtualKeys::NumpadMultiply:
				return VK_MULTIPLY;
			case PlatformInput::VirtualKeys::NumpadDivide:
				return VK_DIVIDE;
			case PlatformInput::VirtualKeys::NumpadDecimal:
				return VK_DECIMAL;
			case PlatformInput::VirtualKeys::F1:
				return VK_F1;
			case PlatformInput::VirtualKeys::F2:
				return VK_F2;
			case PlatformInput::VirtualKeys::F3:
				return VK_F3;
			case PlatformInput::VirtualKeys::F4:
				return VK_F4;
			case PlatformInput::VirtualKeys::F5:
				return VK_F5;
			case PlatformInput::VirtualKeys::F6:
				return VK_F6;
			case PlatformInput::VirtualKeys::F7:
				return VK_F7;
			case PlatformInput::VirtualKeys::F8:
				return VK_F8;
			case PlatformInput::VirtualKeys::F9:
				return VK_F9;
			case PlatformInput::VirtualKeys::F10:
				return VK_F10;
			case PlatformInput::VirtualKeys::F11:
				return VK_F11;
			case PlatformInput::VirtualKeys::F12:
				return VK_F12;
			case PlatformInput::VirtualKeys::GamepadA:
				return VK_GAMEPAD_A;
			case PlatformInput::VirtualKeys::GamepadB:
				return VK_GAMEPAD_B;
			case PlatformInput::VirtualKeys::GamepadX:
				return VK_GAMEPAD_X;
			case PlatformInput::VirtualKeys::GamepadY:
				return VK_GAMEPAD_Y;
			case PlatformInput::VirtualKeys::GamepadRightShoulder:
				return VK_GAMEPAD_RIGHT_SHOULDER;
			case PlatformInput::VirtualKeys::GamepadLeftShoulder:
				return VK_GAMEPAD_LEFT_SHOULDER;
			case PlatformInput::VirtualKeys::GamepadLeftTrigger:
				return VK_GAMEPAD_LEFT_TRIGGER;
			case PlatformInput::VirtualKeys::GamepadRightTrigger:
				return VK_GAMEPAD_RIGHT_TRIGGER;
			case PlatformInput::VirtualKeys::GamepadDPadUp:
				return VK_GAMEPAD_DPAD_UP;
			case PlatformInput::VirtualKeys::GamepadDPadDown:
				return VK_GAMEPAD_DPAD_DOWN;
			case PlatformInput::VirtualKeys::GamepadDPadLeft:
				return VK_GAMEPAD_DPAD_LEFT;
			case PlatformInput::VirtualKeys::GamepadDPadRight:
				return VK_GAMEPAD_DPAD_RIGHT;
			case PlatformInput::VirtualKeys::GamepadMenu:
				return VK_GAMEPAD_MENU;
			case PlatformInput::VirtualKeys::GamepadView:
				return VK_GAMEPAD_VIEW;
			case PlatformInput::VirtualKeys::GamepadLeftThumbstickButton:
				return VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON;
			case PlatformInput::VirtualKeys::GamepadRightThumbstickButton:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON;
			case PlatformInput::VirtualKeys::GamepadLeftThumbstickUp:
				return VK_GAMEPAD_LEFT_THUMBSTICK_UP;
			case PlatformInput::VirtualKeys::GamepadLeftThumbstickDowns:
				return VK_GAMEPAD_LEFT_THUMBSTICK_DOWN;
			case PlatformInput::VirtualKeys::GamepadLeftThumbstickRight:
				return VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT;
			case PlatformInput::VirtualKeys::GamepadLeftThumbstickLeft:
				return VK_GAMEPAD_LEFT_THUMBSTICK_LEFT;
			case PlatformInput::VirtualKeys::GamepadRightThumbstickUp:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_UP;
			case PlatformInput::VirtualKeys::GamepadRightThumbstickDown:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN;
			case PlatformInput::VirtualKeys::GamepadRightThumbstickRight:
				return VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT;
			case PlatformInput::VirtualKeys::GamepadRightThumbstickLeft:
				return VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT;
			}

			return 0;
		}

		bool PlatformInput::GetState(VirtualKeys Key)
		{
			return (GetKeyState(GetVirtualKey(Key)) & 0x8000);
		}

		//void PlatformInput::GetAllStates(bool * KeysState)
		//{
		//	static BYTE state[MAX_KEY_COUNT];

		//	GetKeyboardState(state);

		//	for (uint16 i = 0; i < MAX_KEY_COUNT; ++i)
		//		KeysState[i] = (state[i] & 0x0080);
		//}
	}
}
#endif