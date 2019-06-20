// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
				case KeyCodes::Backspace:
					return PlatformInput::VirtualKeys::Backspace;
				case KeyCodes::Tab:
					return PlatformInput::VirtualKeys::Tab;
				case KeyCodes::KeypadEnter:
					return PlatformInput::VirtualKeys::Enter;
				case KeyCodes::Escape:
					return PlatformInput::VirtualKeys::Escape;
				case KeyCodes::Space:
					return PlatformInput::VirtualKeys::Space;
				case KeyCodes::Comma:
					return PlatformInput::VirtualKeys::Comma;
				case KeyCodes::Minus:
					return PlatformInput::VirtualKeys::OEMMinus;
				case KeyCodes::Period:
					return PlatformInput::VirtualKeys::Period;
				case KeyCodes::Slash:
					return PlatformInput::VirtualKeys::Slash;
				case KeyCodes::Semicolon:
					return PlatformInput::VirtualKeys::Semicolon;
				case KeyCodes::Equals:
					return PlatformInput::VirtualKeys::Equals;
				case KeyCodes::LeftBracket:
					return PlatformInput::VirtualKeys::LeftBracket;
				case KeyCodes::RightBracket:
					return PlatformInput::VirtualKeys::RightBracket;
				case KeyCodes::Backslash:
					return PlatformInput::VirtualKeys::Backslash;
				case KeyCodes::Alpha0:
					return PlatformInput::VirtualKeys::Alpha0;
				case KeyCodes::Alpha1:
					return PlatformInput::VirtualKeys::Alpha1;
				case KeyCodes::Alpha2:
					return PlatformInput::VirtualKeys::Alpha2;
				case KeyCodes::Alpha3:
					return PlatformInput::VirtualKeys::Alpha3;
				case KeyCodes::Alpha4:
					return PlatformInput::VirtualKeys::Alpha4;
				case KeyCodes::Alpha5:
					return PlatformInput::VirtualKeys::Alpha5;
				case KeyCodes::Alpha6:
					return PlatformInput::VirtualKeys::Alpha6;
				case KeyCodes::Alpha7:
					return PlatformInput::VirtualKeys::Alpha7;
				case KeyCodes::Alpha8:
					return PlatformInput::VirtualKeys::Alpha8;
				case KeyCodes::Alpha9:
					return PlatformInput::VirtualKeys::Alpha9;
				case KeyCodes::A:
					return PlatformInput::VirtualKeys::A;
				case KeyCodes::B:
					return PlatformInput::VirtualKeys::B;
				case KeyCodes::C:
					return PlatformInput::VirtualKeys::C;
				case KeyCodes::D:
					return PlatformInput::VirtualKeys::D;
				case KeyCodes::E:
					return PlatformInput::VirtualKeys::E;
				case KeyCodes::F:
					return PlatformInput::VirtualKeys::F;
				case KeyCodes::G:
					return PlatformInput::VirtualKeys::G;
				case KeyCodes::H:
					return PlatformInput::VirtualKeys::H;
				case KeyCodes::I:
					return PlatformInput::VirtualKeys::I;
				case KeyCodes::J:
					return PlatformInput::VirtualKeys::J;
				case KeyCodes::K:
					return PlatformInput::VirtualKeys::K;
				case KeyCodes::L:
					return PlatformInput::VirtualKeys::L;
				case KeyCodes::M:
					return PlatformInput::VirtualKeys::M;
				case KeyCodes::N:
					return PlatformInput::VirtualKeys::N;
				case KeyCodes::O:
					return PlatformInput::VirtualKeys::O;
				case KeyCodes::P:
					return PlatformInput::VirtualKeys::P;
				case KeyCodes::Q:
					return PlatformInput::VirtualKeys::Q;
				case KeyCodes::R:
					return PlatformInput::VirtualKeys::R;
				case KeyCodes::S:
					return PlatformInput::VirtualKeys::S;
				case KeyCodes::T:
					return PlatformInput::VirtualKeys::T;
				case KeyCodes::U:
					return PlatformInput::VirtualKeys::U;
				case KeyCodes::V:
					return PlatformInput::VirtualKeys::V;
				case KeyCodes::W:
					return PlatformInput::VirtualKeys::W;
				case KeyCodes::X:
					return PlatformInput::VirtualKeys::X;
				case KeyCodes::Y:
					return PlatformInput::VirtualKeys::Y;
				case KeyCodes::Z:
					return PlatformInput::VirtualKeys::Z;
				case KeyCodes::Delete:
					return PlatformInput::VirtualKeys::Delete;
				case KeyCodes::Insert:
					return PlatformInput::VirtualKeys::Insert;
				case KeyCodes::Home:
					return PlatformInput::VirtualKeys::Home;
				case KeyCodes::End:
					return PlatformInput::VirtualKeys::End;
				case KeyCodes::PageUp:
					return PlatformInput::VirtualKeys::PageUp;
				case KeyCodes::PageDown:
					return PlatformInput::VirtualKeys::PageDown;
				case KeyCodes::UpArrow:
					return PlatformInput::VirtualKeys::Up;
				case KeyCodes::DownArrow:
					return PlatformInput::VirtualKeys::Down;
				case KeyCodes::RightArrow:
					return PlatformInput::VirtualKeys::Right;
				case KeyCodes::LeftArrow:
					return PlatformInput::VirtualKeys::Left;
				case KeyCodes::KeypadPeriod:
					return PlatformInput::VirtualKeys::Decimal;
				case KeyCodes::KeypadDivide:
					return PlatformInput::VirtualKeys::Divide;
				case KeyCodes::KeypadMultiply:
					return PlatformInput::VirtualKeys::Multiply;
				case KeyCodes::KeypadMinus:
					return PlatformInput::VirtualKeys::Subtract;
				case KeyCodes::KeypadPlus:
					return PlatformInput::VirtualKeys::Add;
				case KeyCodes::KeypadEnter:
					return PlatformInput::VirtualKeys::Enter;
				case KeyCodes::KeypadEquals:
					return PlatformInput::VirtualKeys::KeypadEquals;
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
				case KeyCodes::NumLock:
					return PlatformInput::VirtualKeys::NumLock;
				case KeyCodes::CapsLock:
					return PlatformInput::VirtualKeys::Capital;
				case KeyCodes::ScrollLock:
					return PlatformInput::VirtualKeys::ScrollLock;
				case KeyCodes::RightShift:
					return PlatformInput::VirtualKeys::RightShift;
				case KeyCodes::LeftShift:
					return PlatformInput::VirtualKeys::LeftShift;
				case KeyCodes::RightControl:
					return PlatformInput::VirtualKeys::RightControl;
				case KeyCodes::LeftControl:
					return PlatformInput::VirtualKeys::LeftControl;
				case KeyCodes::RightAlt:
					return PlatformInput::VirtualKeys::RightAlt;
				case KeyCodes::LeftAlt:
					return PlatformInput::VirtualKeys::LeftAlt;
				case KeyCodes::LeftWindows:
					return PlatformInput::VirtualKeys::LeftWindows;
				case KeyCodes::RightWindows:
					return PlatformInput::VirtualKeys::RightWindows;
				case KeyCodes::Mouse0:
					return PlatformInput::VirtualKeys::LeftButton;
				case KeyCodes::Mouse1:
					return PlatformInput::VirtualKeys::RightButton;
				case KeyCodes::Mouse2:
					return PlatformInput::VirtualKeys::MiddleButton;
				case KeyCodes::Mouse3:
					return PlatformInput::VirtualKeys::X1Button;
				case KeyCodes::Mouse4:
					return PlatformInput::VirtualKeys::X2Button;
				case KeyCodes::JoystickButton0:
					return PlatformInput::VirtualKeys::GamepadA;
				case KeyCodes::JoystickButton1:
					return PlatformInput::VirtualKeys::GamepadB;
				case KeyCodes::JoystickButton2:
					return PlatformInput::VirtualKeys::GamepadX;
				case KeyCodes::JoystickButton3:
					return PlatformInput::VirtualKeys::GamepadY;
				case KeyCodes::JoystickButton4:
					return PlatformInput::VirtualKeys::GamepadDPadDown;
				case KeyCodes::JoystickButton5:
					return PlatformInput::VirtualKeys::GamepadDPadLeft;
				case KeyCodes::JoystickButton6:
					return PlatformInput::VirtualKeys::GamepadDPadRight;
				case KeyCodes::JoystickButton7:
					return PlatformInput::VirtualKeys::GamepadDPadUp;
				case KeyCodes::JoystickButton8:
					return PlatformInput::VirtualKeys::GamepadMenu;
				case KeyCodes::JoystickButton9:
					return PlatformInput::VirtualKeys::GamepadView;
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

				PlatformMemory::Set(m_IsKeyDown, 0, len * sizeof(bool));


				PlatformInput::GetState(states);

				if (states['A'])
					m_IsKeyDown[(uint16)KeyCodes::A] = true;


				//PlatformMemory::Copy(m_IsKeyDown, m_WasKeyDown, len * sizeof(bool)); sizes mismatched
			}
		}
	}
}