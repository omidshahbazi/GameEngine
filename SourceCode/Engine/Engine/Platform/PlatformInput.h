// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_INPUT_H
#define PLATFORM_INPUT_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformInput
		{
		public:
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

			//static const uint16 MAX_KEY_COUNT = 256;

		public:
			static bool GetState(VirtualKeys Key);

			//static void GetAllStates(bool *KeysState);
		};
	}
}

#endif