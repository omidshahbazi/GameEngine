// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <InputSystem\Private\KeyboardWrapper.h>
#include <InputSystem\KeyCodes.h>
#include <Platform\PlatformMemory.h>
#include <Platform\PlatformInput.h>

namespace Engine
{
	using namespace Platform;

	namespace InputSystem
	{
		namespace Private
		{
			PlatformInput::VirtualKeys GetVirtualKey(KeyCodes Key)
			{
				switch (Key)
				{
				case KeyCodes::MouseButton1:
					return PlatformInput::VirtualKeys::LeftButton;
				case KeyCodes::MouseButton2:
					return PlatformInput::VirtualKeys::RightButton;
				case KeyCodes::MouseButton3:
					return PlatformInput::VirtualKeys::MiddleButton;
				case KeyCodes::MouseButton4:
					return PlatformInput::VirtualKeys::X1Button;
				case KeyCodes::MouseButton5:
					return PlatformInput::VirtualKeys::X2Button;
				case KeyCodes::Backspace:
					return PlatformInput::VirtualKeys::Backspace;
				case KeyCodes::Tab:
					return PlatformInput::VirtualKeys::Tab;
				case KeyCodes::Return:
					return PlatformInput::VirtualKeys::Return;
				case KeyCodes::Pause:
					return PlatformInput::VirtualKeys::Pause;
				case KeyCodes::Escape:
					return PlatformInput::VirtualKeys::Escape;
				case KeyCodes::Space:
					return PlatformInput::VirtualKeys::Space;
				case KeyCodes::PageUp:
					return PlatformInput::VirtualKeys::PageUp;
				case KeyCodes::PageDown:
					return PlatformInput::VirtualKeys::PageDown;
				case KeyCodes::End:
					return PlatformInput::VirtualKeys::End;
				case KeyCodes::Home:
					return PlatformInput::VirtualKeys::Home;
				case KeyCodes::Up:
					return PlatformInput::VirtualKeys::Up;
				case KeyCodes::Down:
					return PlatformInput::VirtualKeys::Down;
				case KeyCodes::Left:
					return PlatformInput::VirtualKeys::Left;
				case KeyCodes::Right:
					return PlatformInput::VirtualKeys::Right;
				case KeyCodes::PrintScreen:
					return PlatformInput::VirtualKeys::PrintScreen;
				case KeyCodes::Insert:
					return PlatformInput::VirtualKeys::Insert;
				case KeyCodes::Delete:
					return PlatformInput::VirtualKeys::Delete;
				case KeyCodes::ContextMenu:
					return PlatformInput::VirtualKeys::ContextMenu;
				case KeyCodes::Capital:
					return PlatformInput::VirtualKeys::Capital;
				case KeyCodes::NumLock:
					return PlatformInput::VirtualKeys::NumLock;
				case KeyCodes::ScrollLock:
					return PlatformInput::VirtualKeys::ScrollLock;
				case KeyCodes::Shift:
					return PlatformInput::VirtualKeys::Shift;
				case KeyCodes::LeftShift:
					return PlatformInput::VirtualKeys::LeftShift;
				case KeyCodes::RightShift:
					return PlatformInput::VirtualKeys::RightShift;
				case KeyCodes::Control:
					return PlatformInput::VirtualKeys::Control;
				case KeyCodes::LeftControl:
					return PlatformInput::VirtualKeys::LeftControl;
				case KeyCodes::RightControl:
					return PlatformInput::VirtualKeys::RightControl;
				case KeyCodes::Alt:
					return PlatformInput::VirtualKeys::Alt;
				case KeyCodes::LeftAlt:
					return PlatformInput::VirtualKeys::LeftAlt;
				case KeyCodes::RightAlt:
					return PlatformInput::VirtualKeys::RightAlt;
				case KeyCodes::LeftWindows:
					return PlatformInput::VirtualKeys::LeftWindows;
				case KeyCodes::RightWindows:
					return PlatformInput::VirtualKeys::RightWindows;
				case KeyCodes::KeypadApostrophe:
					return PlatformInput::VirtualKeys::KeypadApostrophe;
				case KeyCodes::KeypadMinus:
					return PlatformInput::VirtualKeys::KeypadMinus;
				case KeyCodes::KeypadPlus:
					return PlatformInput::VirtualKeys::KeypadPlus;
				case KeyCodes::KeypadLeftSquareBraket:
					return PlatformInput::VirtualKeys::KeypadLeftSquareBraket;
				case KeyCodes::KeypadRightSquareBraket:
					return PlatformInput::VirtualKeys::KeypadRightSquareBraket;
				case KeyCodes::KeypadColon:
					return PlatformInput::VirtualKeys::KeypadColon;
				case KeyCodes::KeypadQuotation:
					return PlatformInput::VirtualKeys::KeypadQuotation;
				case KeyCodes::KeypadBackSlash:
					return PlatformInput::VirtualKeys::KeypadBackSlash;
				case KeyCodes::KeypadComma:
					return PlatformInput::VirtualKeys::KeypadComma;
				case KeyCodes::KeypadDot:
					return PlatformInput::VirtualKeys::KeypadDot;
				case KeyCodes::KeypadForwardSlash:
					return PlatformInput::VirtualKeys::KeypadForwardSlash;
				case KeyCodes::Keypad0:
					return PlatformInput::VirtualKeys::Keypad0;
				case KeyCodes::Keypad1:
					return PlatformInput::VirtualKeys::Keypad1;
				case KeyCodes::Keypad2:
					return PlatformInput::VirtualKeys::Keypad2;
				case KeyCodes::Keypad3:
					return PlatformInput::VirtualKeys::Keypad3;
				case KeyCodes::Keypad4:
					return PlatformInput::VirtualKeys::Keypad4;
				case KeyCodes::Keypad5:
					return PlatformInput::VirtualKeys::Keypad5;
				case KeyCodes::Keypad6:
					return PlatformInput::VirtualKeys::Keypad6;
				case KeyCodes::Keypad7:
					return PlatformInput::VirtualKeys::Keypad7;
				case KeyCodes::Keypad8:
					return PlatformInput::VirtualKeys::Keypad8;
				case KeyCodes::Keypad9:
					return PlatformInput::VirtualKeys::Keypad9;
				case KeyCodes::KeypadA:
					return PlatformInput::VirtualKeys::KeypadA;
				case KeyCodes::KeypadB:
					return PlatformInput::VirtualKeys::KeypadB;
				case KeyCodes::KeypadC:
					return PlatformInput::VirtualKeys::KeypadC;
				case KeyCodes::KeypadD:
					return PlatformInput::VirtualKeys::KeypadD;
				case KeyCodes::KeypadE:
					return PlatformInput::VirtualKeys::KeypadE;
				case KeyCodes::KeypadF:
					return PlatformInput::VirtualKeys::KeypadF;
				case KeyCodes::KeypadG:
					return PlatformInput::VirtualKeys::KeypadG;
				case KeyCodes::KeypadH:
					return PlatformInput::VirtualKeys::KeypadH;
				case KeyCodes::KeypadI:
					return PlatformInput::VirtualKeys::KeypadI;
				case KeyCodes::KeypadJ:
					return PlatformInput::VirtualKeys::KeypadJ;
				case KeyCodes::KeypadK:
					return PlatformInput::VirtualKeys::KeypadK;
				case KeyCodes::KeypadL:
					return PlatformInput::VirtualKeys::KeypadL;
				case KeyCodes::KeypadM:
					return PlatformInput::VirtualKeys::KeypadM;
				case KeyCodes::KeypadN:
					return PlatformInput::VirtualKeys::KeypadN;
				case KeyCodes::KeypadO:
					return PlatformInput::VirtualKeys::KeypadO;
				case KeyCodes::KeypadP:
					return PlatformInput::VirtualKeys::KeypadP;
				case KeyCodes::KeypadQ:
					return PlatformInput::VirtualKeys::KeypadQ;
				case KeyCodes::KeypadR:
					return PlatformInput::VirtualKeys::KeypadR;
				case KeyCodes::KeypadS:
					return PlatformInput::VirtualKeys::KeypadS;
				case KeyCodes::KeypadT:
					return PlatformInput::VirtualKeys::KeypadT;
				case KeyCodes::KeypadU:
					return PlatformInput::VirtualKeys::KeypadU;
				case KeyCodes::KeypadV:
					return PlatformInput::VirtualKeys::KeypadV;
				case KeyCodes::KeypadW:
					return PlatformInput::VirtualKeys::KeypadW;
				case KeyCodes::KeypadX:
					return PlatformInput::VirtualKeys::KeypadX;
				case KeyCodes::KeypadY:
					return PlatformInput::VirtualKeys::KeypadY;
				case KeyCodes::KeypadZ:
					return PlatformInput::VirtualKeys::KeypadZ;
				case KeyCodes::Numpad0:
					return PlatformInput::VirtualKeys::Numpad0;
				case KeyCodes::Numpad1:
					return PlatformInput::VirtualKeys::Numpad1;
				case KeyCodes::Numpad2:
					return PlatformInput::VirtualKeys::Numpad2;
				case KeyCodes::Numpad3:
					return PlatformInput::VirtualKeys::Numpad3;
				case KeyCodes::Numpad4:
					return PlatformInput::VirtualKeys::Numpad4;
				case KeyCodes::Numpad5:
					return PlatformInput::VirtualKeys::Numpad5;
				case KeyCodes::Numpad6:
					return PlatformInput::VirtualKeys::Numpad6;
				case KeyCodes::Numpad7:
					return PlatformInput::VirtualKeys::Numpad7;
				case KeyCodes::Numpad8:
					return PlatformInput::VirtualKeys::Numpad8;
				case KeyCodes::Numpad9:
					return PlatformInput::VirtualKeys::Numpad9;
				case KeyCodes::NumpadAdd:
					return PlatformInput::VirtualKeys::NumpadAdd;
				case KeyCodes::NumpadSubtract:
					return PlatformInput::VirtualKeys::NumpadSubtract;
				case KeyCodes::NumpadMultiply:
					return PlatformInput::VirtualKeys::NumpadMultiply;
				case KeyCodes::NumpadDivide:
					return PlatformInput::VirtualKeys::NumpadDivide;
				case KeyCodes::NumpadDecimal:
					return PlatformInput::VirtualKeys::NumpadDecimal;
				case KeyCodes::F1:
					return PlatformInput::VirtualKeys::F1;
				case KeyCodes::F2:
					return PlatformInput::VirtualKeys::F2;
				case KeyCodes::F3:
					return PlatformInput::VirtualKeys::F3;
				case KeyCodes::F4:
					return PlatformInput::VirtualKeys::F4;
				case KeyCodes::F5:
					return PlatformInput::VirtualKeys::F5;
				case KeyCodes::F6:
					return PlatformInput::VirtualKeys::F6;
				case KeyCodes::F7:
					return PlatformInput::VirtualKeys::F7;
				case KeyCodes::F8:
					return PlatformInput::VirtualKeys::F8;
				case KeyCodes::F9:
					return PlatformInput::VirtualKeys::F9;
				case KeyCodes::F10:
					return PlatformInput::VirtualKeys::F10;
				case KeyCodes::F11:
					return PlatformInput::VirtualKeys::F11;
				case KeyCodes::F12:
					return PlatformInput::VirtualKeys::F12;
				}

				return (PlatformInput::VirtualKeys)0;
			}

			KeyboardWrapper::KeyboardWrapper(bool *WasKeyDown, bool *IsKeyDown) :
				m_WasKeyDown(WasKeyDown),
				m_IsKeyDown(IsKeyDown)
			{
			}

			KeyboardWrapper::~KeyboardWrapper(void)
			{
			}

			void KeyboardWrapper::Update(void)
			{
				uint16 len = (uint16)KeyCodes::COUNT;

				PlatformMemory::Copy(m_IsKeyDown, m_WasKeyDown, len * sizeof(bool));

				for (KeyCodes key = KeyCodes::MouseButton1; key != KeyCodes::COUNT; key = (KeyCodes)((int32)key + 1))
					m_IsKeyDown[(int32)key] = PlatformInput::GetState(GetVirtualKey(key));
			}
		}
	}
}