// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WindowUtility\Window.h>
#include <Common\BitwiseUtils.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;

	namespace WindowUtility
	{
		const cstr CLASS_NAME = "EngineWindow";
		const int32 ACTIVE_BORDER_WIDTH = 5;

#define IS_STYLE_SET(Style) ((PlatformWindow::GetStyle(m_Handle) & Style) == Style)
#define SET_STYLE_STATE(Style, Enabled) \
		{ \
			PlatformWindow::Styles style = PlatformWindow::GetStyle(m_Handle); \
			if (Enabled) \
				style |= Style; \
			else \
				style &= ~Style; \
			PlatformWindow::SetStyle(m_Handle, style); \
		}

#define IS_EXTRA_STYLE_SET(Style) ((PlatformWindow::GetExtraStyle(m_Handle) & Style) == Style)
#define SET_EXTRA_STYLE_STATE(Style, Enabled) \
		{ \
			PlatformWindow::ExtraStyles style = PlatformWindow::GetExtraStyle(m_Handle); \
			if (Enabled) \
				style |= Style; \
			else \
				style &= ~Style; \
			PlatformWindow::SetExtraStyle(m_Handle, style); \
		}

		Window::Window(const String& Name) :
			m_Handle(0),
			m_Name(Name),
			m_IsFocused(false),
			m_IsFixedSize(false),
			m_SystemMenuEnabled(true),
			m_SystemMenuWidth(25),
			m_TitleBarEnabled(true),
			m_TitleBarSize(-1, 25),
			m_ShowFrame(true),
			m_State(States::Noraml),
			m_BorderStyle(BorderStyles::Normal),
			m_IsClosed(false)
		{
		}

		Window::~Window(void)
		{
			if (m_Handle != 0)
				PlatformWindow::Destroy(m_Handle);
		}

		bool Window::Initialize(void)
		{
			static bool isPlatformWindowInitialized = false;
			if (!isPlatformWindowInitialized)
			{
				CoreDebugAssert(Categories::Utilities, PlatformWindow::Initialize(PlatformOS::GetExecutingModuleInstance(), CLASS_NAME), "PlatformWindow initialization failed");

				isPlatformWindowInitialized = true;
			}

			m_Handle = PlatformWindow::Create(CLASS_NAME, m_Name.GetValue(), [&](PlatformWindow::WindowMessages Message, void* Parameter) { return MessageProcedure(Message, Parameter); });

			PlatformWindow::SetStyle(m_Handle, PlatformWindow::Styles::Overlapped | PlatformWindow::Styles::Caption | PlatformWindow::Styles::SystemMenu | PlatformWindow::Styles::ThickFrame | PlatformWindow::Styles::MinimizeBox | PlatformWindow::Styles::MaximizeBox);

			SetTitle(m_Name);
			SetMinimumSize({ 1, 1 });
			SetMaximumSize({ 10000, 10000 });
			SetSize({ 100, 100 });
			SetIsVisible(true);

			return false;
		}

		void Window::SetTitle(const String& Value)
		{
			THROW_IF_EXCEPTION(Categories::Utilities, m_Handle == 0, "Window doesn't initialized");

			m_Title = Value;

			PlatformWindow::SetTitle(m_Handle, Value.GetValue());
		}

		void Window::SetPosition(const Vector2I& Value)
		{
			THROW_IF_EXCEPTION(Categories::Utilities, m_Handle == 0, "Window doesn't initialized");

			m_Position = Value;

			PlatformWindow::SetPosition(m_Handle, m_Position.X, m_Position.Y);
		}

		void Window::SetSize(const Vector2I& Value)
		{
			THROW_IF_EXCEPTION(Categories::Utilities, m_Handle == 0, "Window doesn't initialized");

			PlatformWindow::SetSize(m_Handle, Value.X, Value.Y);
		}

		bool Window::GetIsVisible(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Visible);
		}

		void Window::SetIsVisible(bool Value)
		{
			PlatformWindow::ShowWindow(m_Handle, (Value ? PlatformWindow::ShowWindowStates::Show : PlatformWindow::ShowWindowStates::Hide));

			PlatformWindow::Update(m_Handle);
		}

		bool Window::GetIsDisabled(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Disabled);
		}

		void Window::SetIsDisabled(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Disabled, Value);
		}

		bool Window::GetShowCaption(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Caption);
		}

		void Window::SetShowCaption(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Caption, Value);
		}

		bool Window::GetShowSystemMenu(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::SystemMenu);
		}

		void Window::SetShowSystemMenu(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::SystemMenu, Value);
		}

		bool Window::GetShowBorder(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Border);
		}

		void Window::SetShowBorder(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Border, Value);
		}

		bool Window::GetIsTabStop(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::TabStop);
		}

		void Window::SetIsTabStop(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::TabStop, Value);
		}

		bool Window::GetShowMinimizeBox(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::MinimizeBox);
		}

		void Window::SetShowMinimizeBox(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::MinimizeBox, Value);
		}

		bool Window::GetShowMaximizeBox(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::MaximizeBox);
		}

		void Window::SetShowMaximizeBox(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::MaximizeBox, Value);
		}

		void Window::SetShowFrame(bool Value)
		{
			m_ShowFrame = Value;

			PlatformWindow::Update(m_Handle);

			UpdateSize(true);
		}

		void Window::Close(void) const
		{
			PlatformWindow::Close(m_Handle);
		}

		bool Window::GetIsTopMost(void) const
		{
			return IS_EXTRA_STYLE_SET(PlatformWindow::ExtraStyles::TopMost);
		}

		void Window::SetIsTopMost(bool Value)
		{
			SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::TopMost, Value);
			PlatformWindow::SetPositionOption(m_Handle, Value ? PlatformWindow::Places::TopMost : PlatformWindow::Places::NoTopMost);
		}

		bool Window::GetAcceptFiles(void) const
		{
			return IS_EXTRA_STYLE_SET(PlatformWindow::ExtraStyles::AcceptFiles);
		}

		void Window::SetAcceptFiles(bool Value)
		{
			SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::AcceptFiles, Value);
		}

		void Window::SetState(States Value)
		{
			switch (Value)
			{
			case States::Noraml:
				PlatformWindow::ShowWindow(m_Handle, PlatformWindow::ShowWindowStates::ShowNormal);
				break;

			case States::Minimized:
				PlatformWindow::ShowWindow(m_Handle, PlatformWindow::ShowWindowStates::ShowMinimized);
				break;

			case States::Maximized:
				PlatformWindow::ShowWindow(m_Handle, PlatformWindow::ShowWindowStates::Maximize);
				break;
			}
		}

		void Window::SetBorderStyle(BorderStyles Value)
		{
			m_BorderStyle = Value;

			switch (Value)
			{
			case BorderStyles::Normal:
				SET_STYLE_STATE(PlatformWindow::Styles::ThickFrame, false);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::ToolWindow, false);
				SET_STYLE_STATE(PlatformWindow::Styles::DialogFrame, true);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::DialogModalFrame, true);
				break;

			case BorderStyles::Tool:
				SET_STYLE_STATE(PlatformWindow::Styles::DialogFrame, false);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::DialogModalFrame, false);
				SET_STYLE_STATE(PlatformWindow::Styles::ThickFrame, true);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::ToolWindow, true);
				break;
			}
		}

		void Window::UpdateSize(bool Force)
		{
			uint16 x;
			uint16 y;

			PlatformWindow::GetSize(m_Handle, x, y);
			if (Force || x != m_Size.X || y != m_Size.Y)
			{
				m_Size.X = x;
				m_Size.Y = y;

				if (GetShowFrame())
					PlatformWindow::GetClientSize(m_Handle, x, y);

				m_ClientSize.X = x;
				m_ClientSize.Y = y;

				OnSizeChangedEvent(this);
			}
		}

		bool Window::MessageProcedure(PlatformWindow::WindowMessages Message, void* Parameter)
		{
			switch (Message)
			{
			case PlatformWindow::WindowMessages::Activate:
			{
				PlatformWindow::ActivateInfo* info = ReinterpretCast(PlatformWindow::ActivateInfo*, Parameter);

				m_IsFocused = info->Focused;
			} break;
			case PlatformWindow::WindowMessages::Resized:
			{
				PlatformWindow::ResizedInfo* info = ReinterpretCast(PlatformWindow::ResizedInfo*, Parameter);

				if (info->State == PlatformWindow::ResizedStates::Minimized)
					m_State = States::Minimized;
				else if (info->State == PlatformWindow::ResizedStates::Maximized)
					m_State = States::Maximized;
				else
					m_State = States::Noraml;

				if (m_State != States::Minimized)
					UpdateSize(false);
			} break;
			case PlatformWindow::WindowMessages::Moved:
			{
				int16 x;
				int16 y;

				PlatformWindow::GetClientPosition(m_Handle, x, y);
				if (x != m_ClientPosition.X || y != m_ClientPosition.Y)
				{
					m_ClientPosition.X = x;
					m_ClientPosition.Y = y;

					PlatformWindow::GetPosition(m_Handle, x, y);
					m_Position.X = x;
					m_Position.Y = y;

					OnPositionChangedEvent(this);
				}
			} break;
			case PlatformWindow::WindowMessages::GetMinMaxInfo:
			{
				PlatformWindow::DisplayHandle display = PlatformWindow::GetDisplay(m_Handle);

				PlatformWindow::DisplayInfo displayInfo;
				PlatformWindow::GetDisplayInfo(display, &displayInfo);

				PlatformWindow::MinMaxSizeInfo* info = ReinterpretCast(PlatformWindow::MinMaxSizeInfo*, Parameter);

				info->MaxX = abs(displayInfo.WorkX - displayInfo.DisplayX);
				info->MaxY = abs(displayInfo.WorkY - displayInfo.DisplayY);
				info->MinWidth = m_MinimumSize.X;
				info->MinHeight = m_MinimumSize.Y;

				if (PlatformWindow::GetWindowState(m_Handle) == PlatformWindow::ShowWindowStates::Maximize)
				{
					info->MaxWidth = Math::Min(m_MaximumSize.X, displayInfo.WorkWidth);
					info->MaxHeight = Math::Min(m_MaximumSize.Y, displayInfo.WorkHeight - 1);
				}
				else
				{
					info->MaxWidth = m_MaximumSize.X;
					info->MaxHeight = m_MaximumSize.Y;
				}

				return true;
			} break;
			case PlatformWindow::WindowMessages::KeyUp:
			{
				PlatformWindow::KeyInfo* info = ReinterpretCast(PlatformWindow::KeyInfo*, Parameter);

				for (uint16 i = 0; i < info->RepeatCount; ++i)
					OnKeyUpEvent(this, info->Key);
			} break;
			case PlatformWindow::WindowMessages::KeyDown:
			{
				PlatformWindow::KeyInfo* info = ReinterpretCast(PlatformWindow::KeyInfo*, Parameter);

				OnKeyDownEvent(this, info->Key);

				OnKeyPressedEvent(this, info->Key);
			} break;
			case PlatformWindow::WindowMessages::MouseDown:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				OnMouseDownEvent(this, info->Key, Vector2I(info->X, info->Y));
			} break;
			case PlatformWindow::WindowMessages::MouseUp:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				OnMouseUpEvent(this, info->Key, Vector2I(info->X, info->Y));

				OnMouseClickEvent(this, info->Key, Vector2I(info->X, info->Y));
			} break;
			case PlatformWindow::WindowMessages::MouseWheel:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				OnMouseWheelEvent(this, Vector2I(info->X, info->Y), info->WheelDelta);
			} break;
			case PlatformWindow::WindowMessages::MouseMove:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				OnMouseMoveEvent(this, Vector2I(info->X, info->Y));
			} break;
			case PlatformWindow::WindowMessages::MouseLeave:
			{
				OnMouseLeaveEvent(this);
			} break;
			case PlatformWindow::WindowMessages::CalculateSize:
			{
				if (!m_ShowFrame)
					return true;
			} break;
			case PlatformWindow::WindowMessages::HitTest:
			{
				PlatformWindow::HitTestInfo* info = ReinterpretCast(PlatformWindow::HitTestInfo*, Parameter);

				if (m_ShowFrame)
					return false;

				info->Point = PlatformWindow::HitPoints::None;

				int32 x = info->X - m_Position.X;
				int32 y = info->Y - m_Position.Y;

				int32 activeBorder = m_IsFixedSize ? 0 : ACTIVE_BORDER_WIDTH;

				if (!m_IsFixedSize && m_State != States::Maximized)
				{
					if (x < activeBorder && y < activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::TopLeft;

						return true;
					}
					else if (x > m_Size.X - activeBorder && y < activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::TopRight;

						return true;
					}
					else if (x < activeBorder && y > m_Size.Y - activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::BottomLeft;

						return true;
					}
					else if (x > m_Size.X - activeBorder && y > m_Size.Y - activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::BottomRight;

						return true;
					}

					else if (x < activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::Left;

						return true;
					}
					else if (y < activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::Top;

						return true;
					}
					else if (x > m_Size.X - activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::Right;

						return true;
					}
					else if (y > m_Size.Y - activeBorder)
					{
						info->Point = PlatformWindow::HitPoints::Bottom;

						return true;
					}
				}

				RectI testRect;
				testRect.Position.X = (m_SystemMenuEnabled ? m_SystemMenuWidth : 0) + activeBorder;
				testRect.Position.Y = activeBorder;
				testRect.Size.X = (m_TitleBarSize.X == -1 ? m_Size.X : m_TitleBarSize.X) - (m_SystemMenuEnabled ? m_SystemMenuWidth : 0);
				testRect.Size.Y = m_TitleBarSize.Y - activeBorder;

				if (m_TitleBarEnabled && testRect.Contains({ x, y }))
				{
					info->Point = PlatformWindow::HitPoints::Caption;

					return true;
				}

				if (m_SystemMenuEnabled &&
					activeBorder < x && x < testRect.Position.X &&
					activeBorder < y && y < m_TitleBarSize.Y)
				{
					info->Point = PlatformWindow::HitPoints::SystemMenu;

					return true;
				}

				if (x < m_Size.X && y < m_Size.Y)
				{
					info->Point = PlatformWindow::HitPoints::Client;

					return true;
				}

				return false;
			} break;
			case PlatformWindow::WindowMessages::Close:
			{
				m_IsClosed = true;

				OnClosingEvent(this);

			} break;
			}

			return false;
		}
	}
}