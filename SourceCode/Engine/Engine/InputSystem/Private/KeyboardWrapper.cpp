// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <InputSystem\Private\KeyboardWrapper.h>
#include <InputSystem\KeyCodes.h>
#include <Platform\PlatformMemory.h>
#include <Platform\PlatformWindow.h>

namespace Engine
{
	using namespace Platform;

	namespace InputSystem
	{
		namespace Private
		{
			PlatformWindow::VirtualKeys GetVirtualKey(KeyCodes Key)
			{
				switch (Key)
				{
				case KeyCodes::MouseButton1:
					return PlatformWindow::VirtualKeys::LeftButton;
				case KeyCodes::MouseButton2:
					return PlatformWindow::VirtualKeys::RightButton;
				case KeyCodes::MouseButton3:
					return PlatformWindow::VirtualKeys::MiddleButton;
				case KeyCodes::MouseButton4:
					return PlatformWindow::VirtualKeys::X1Button;
				case KeyCodes::MouseButton5:
					return PlatformWindow::VirtualKeys::X2Button;
				case KeyCodes::Backspace:
					return PlatformWindow::VirtualKeys::Backspace;
				case KeyCodes::Tab:
					return PlatformWindow::VirtualKeys::Tab;
				case KeyCodes::Return:
					return PlatformWindow::VirtualKeys::Return;
				case KeyCodes::Pause:
					return PlatformWindow::VirtualKeys::Pause;
				case KeyCodes::Escape:
					return PlatformWindow::VirtualKeys::Escape;
				case KeyCodes::Space:
					return PlatformWindow::VirtualKeys::Space;
				case KeyCodes::PageUp:
					return PlatformWindow::VirtualKeys::PageUp;
				case KeyCodes::PageDown:
					return PlatformWindow::VirtualKeys::PageDown;
				case KeyCodes::End:
					return PlatformWindow::VirtualKeys::End;
				case KeyCodes::Home:
					return PlatformWindow::VirtualKeys::Home;
				case KeyCodes::Up:
					return PlatformWindow::VirtualKeys::Up;
				case KeyCodes::Down:
					return PlatformWindow::VirtualKeys::Down;
				case KeyCodes::Left:
					return PlatformWindow::VirtualKeys::Left;
				case KeyCodes::Right:
					return PlatformWindow::VirtualKeys::Right;
				case KeyCodes::PrintScreen:
					return PlatformWindow::VirtualKeys::PrintScreen;
				case KeyCodes::Insert:
					return PlatformWindow::VirtualKeys::Insert;
				case KeyCodes::Delete:
					return PlatformWindow::VirtualKeys::Delete;
				case KeyCodes::ContextMenu:
					return PlatformWindow::VirtualKeys::ContextMenu;
				case KeyCodes::Capital:
					return PlatformWindow::VirtualKeys::Capital;
				case KeyCodes::NumLock:
					return PlatformWindow::VirtualKeys::NumLock;
				case KeyCodes::ScrollLock:
					return PlatformWindow::VirtualKeys::ScrollLock;
				case KeyCodes::Shift:
					return PlatformWindow::VirtualKeys::Shift;
				case KeyCodes::LeftShift:
					return PlatformWindow::VirtualKeys::LeftShift;
				case KeyCodes::RightShift:
					return PlatformWindow::VirtualKeys::RightShift;
				case KeyCodes::Control:
					return PlatformWindow::VirtualKeys::Control;
				case KeyCodes::LeftControl:
					return PlatformWindow::VirtualKeys::LeftControl;
				case KeyCodes::RightControl:
					return PlatformWindow::VirtualKeys::RightControl;
				case KeyCodes::Alt:
					return PlatformWindow::VirtualKeys::Alt;
				case KeyCodes::LeftAlt:
					return PlatformWindow::VirtualKeys::LeftAlt;
				case KeyCodes::RightAlt:
					return PlatformWindow::VirtualKeys::RightAlt;
				case KeyCodes::LeftWindows:
					return PlatformWindow::VirtualKeys::LeftWindows;
				case KeyCodes::RightWindows:
					return PlatformWindow::VirtualKeys::RightWindows;
				case KeyCodes::KeypadApostrophe:
					return PlatformWindow::VirtualKeys::KeypadApostrophe;
				case KeyCodes::KeypadMinus:
					return PlatformWindow::VirtualKeys::KeypadMinus;
				case KeyCodes::KeypadPlus:
					return PlatformWindow::VirtualKeys::KeypadPlus;
				case KeyCodes::KeypadLeftSquareBraket:
					return PlatformWindow::VirtualKeys::KeypadLeftSquareBraket;
				case KeyCodes::KeypadRightSquareBraket:
					return PlatformWindow::VirtualKeys::KeypadRightSquareBraket;
				case KeyCodes::KeypadColon:
					return PlatformWindow::VirtualKeys::KeypadColon;
				case KeyCodes::KeypadQuotation:
					return PlatformWindow::VirtualKeys::KeypadQuotation;
				case KeyCodes::KeypadBackSlash:
					return PlatformWindow::VirtualKeys::KeypadBackSlash;
				case KeyCodes::KeypadComma:
					return PlatformWindow::VirtualKeys::KeypadComma;
				case KeyCodes::KeypadDot:
					return PlatformWindow::VirtualKeys::KeypadDot;
				case KeyCodes::KeypadForwardSlash:
					return PlatformWindow::VirtualKeys::KeypadForwardSlash;
				case KeyCodes::Keypad0:
					return PlatformWindow::VirtualKeys::Keypad0;
				case KeyCodes::Keypad1:
					return PlatformWindow::VirtualKeys::Keypad1;
				case KeyCodes::Keypad2:
					return PlatformWindow::VirtualKeys::Keypad2;
				case KeyCodes::Keypad3:
					return PlatformWindow::VirtualKeys::Keypad3;
				case KeyCodes::Keypad4:
					return PlatformWindow::VirtualKeys::Keypad4;
				case KeyCodes::Keypad5:
					return PlatformWindow::VirtualKeys::Keypad5;
				case KeyCodes::Keypad6:
					return PlatformWindow::VirtualKeys::Keypad6;
				case KeyCodes::Keypad7:
					return PlatformWindow::VirtualKeys::Keypad7;
				case KeyCodes::Keypad8:
					return PlatformWindow::VirtualKeys::Keypad8;
				case KeyCodes::Keypad9:
					return PlatformWindow::VirtualKeys::Keypad9;
				case KeyCodes::KeypadA:
					return PlatformWindow::VirtualKeys::KeypadA;
				case KeyCodes::KeypadB:
					return PlatformWindow::VirtualKeys::KeypadB;
				case KeyCodes::KeypadC:
					return PlatformWindow::VirtualKeys::KeypadC;
				case KeyCodes::KeypadD:
					return PlatformWindow::VirtualKeys::KeypadD;
				case KeyCodes::KeypadE:
					return PlatformWindow::VirtualKeys::KeypadE;
				case KeyCodes::KeypadF:
					return PlatformWindow::VirtualKeys::KeypadF;
				case KeyCodes::KeypadG:
					return PlatformWindow::VirtualKeys::KeypadG;
				case KeyCodes::KeypadH:
					return PlatformWindow::VirtualKeys::KeypadH;
				case KeyCodes::KeypadI:
					return PlatformWindow::VirtualKeys::KeypadI;
				case KeyCodes::KeypadJ:
					return PlatformWindow::VirtualKeys::KeypadJ;
				case KeyCodes::KeypadK:
					return PlatformWindow::VirtualKeys::KeypadK;
				case KeyCodes::KeypadL:
					return PlatformWindow::VirtualKeys::KeypadL;
				case KeyCodes::KeypadM:
					return PlatformWindow::VirtualKeys::KeypadM;
				case KeyCodes::KeypadN:
					return PlatformWindow::VirtualKeys::KeypadN;
				case KeyCodes::KeypadO:
					return PlatformWindow::VirtualKeys::KeypadO;
				case KeyCodes::KeypadP:
					return PlatformWindow::VirtualKeys::KeypadP;
				case KeyCodes::KeypadQ:
					return PlatformWindow::VirtualKeys::KeypadQ;
				case KeyCodes::KeypadR:
					return PlatformWindow::VirtualKeys::KeypadR;
				case KeyCodes::KeypadS:
					return PlatformWindow::VirtualKeys::KeypadS;
				case KeyCodes::KeypadT:
					return PlatformWindow::VirtualKeys::KeypadT;
				case KeyCodes::KeypadU:
					return PlatformWindow::VirtualKeys::KeypadU;
				case KeyCodes::KeypadV:
					return PlatformWindow::VirtualKeys::KeypadV;
				case KeyCodes::KeypadW:
					return PlatformWindow::VirtualKeys::KeypadW;
				case KeyCodes::KeypadX:
					return PlatformWindow::VirtualKeys::KeypadX;
				case KeyCodes::KeypadY:
					return PlatformWindow::VirtualKeys::KeypadY;
				case KeyCodes::KeypadZ:
					return PlatformWindow::VirtualKeys::KeypadZ;
				case KeyCodes::Numpad0:
					return PlatformWindow::VirtualKeys::Numpad0;
				case KeyCodes::Numpad1:
					return PlatformWindow::VirtualKeys::Numpad1;
				case KeyCodes::Numpad2:
					return PlatformWindow::VirtualKeys::Numpad2;
				case KeyCodes::Numpad3:
					return PlatformWindow::VirtualKeys::Numpad3;
				case KeyCodes::Numpad4:
					return PlatformWindow::VirtualKeys::Numpad4;
				case KeyCodes::Numpad5:
					return PlatformWindow::VirtualKeys::Numpad5;
				case KeyCodes::Numpad6:
					return PlatformWindow::VirtualKeys::Numpad6;
				case KeyCodes::Numpad7:
					return PlatformWindow::VirtualKeys::Numpad7;
				case KeyCodes::Numpad8:
					return PlatformWindow::VirtualKeys::Numpad8;
				case KeyCodes::Numpad9:
					return PlatformWindow::VirtualKeys::Numpad9;
				case KeyCodes::NumpadAdd:
					return PlatformWindow::VirtualKeys::NumpadAdd;
				case KeyCodes::NumpadSubtract:
					return PlatformWindow::VirtualKeys::NumpadSubtract;
				case KeyCodes::NumpadMultiply:
					return PlatformWindow::VirtualKeys::NumpadMultiply;
				case KeyCodes::NumpadDivide:
					return PlatformWindow::VirtualKeys::NumpadDivide;
				case KeyCodes::NumpadDecimal:
					return PlatformWindow::VirtualKeys::NumpadDecimal;
				case KeyCodes::F1:
					return PlatformWindow::VirtualKeys::F1;
				case KeyCodes::F2:
					return PlatformWindow::VirtualKeys::F2;
				case KeyCodes::F3:
					return PlatformWindow::VirtualKeys::F3;
				case KeyCodes::F4:
					return PlatformWindow::VirtualKeys::F4;
				case KeyCodes::F5:
					return PlatformWindow::VirtualKeys::F5;
				case KeyCodes::F6:
					return PlatformWindow::VirtualKeys::F6;
				case KeyCodes::F7:
					return PlatformWindow::VirtualKeys::F7;
				case KeyCodes::F8:
					return PlatformWindow::VirtualKeys::F8;
				case KeyCodes::F9:
					return PlatformWindow::VirtualKeys::F9;
				case KeyCodes::F10:
					return PlatformWindow::VirtualKeys::F10;
				case KeyCodes::F11:
					return PlatformWindow::VirtualKeys::F11;
				case KeyCodes::F12:
					return PlatformWindow::VirtualKeys::F12;
				}

				return (PlatformWindow::VirtualKeys)0;
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
					m_IsKeyDown[(int32)key] = PlatformWindow::GetState(GetVirtualKey(key));
			}
		}
	}
}