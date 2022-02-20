// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <OpenGLRenderDevice\Private\OpenGLFence.h>

namespace Engine
{
	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			OpenGLFence::OpenGLFence(OpenGLDevice* Device) :
				m_Value(0),
				m_DesiredValue(0),
				m_Event(0)
			{
				m_Event = PlatformOS::CreateSignalEvent();
			}

			OpenGLFence::~OpenGLFence(void)
			{
				PlatformOS::DestroySignalEvent(m_Event);
			}

			bool OpenGLFence::GetIsPassed(void)
			{
				return (m_Value >= m_DesiredValue);
			}

			bool OpenGLFence::Wait(void)
			{
				if (GetIsPassed())
					return true;

				PlatformOS::WaitForSignalEvent(m_Event);

				return true;
			}

			const uint64& OpenGLFence::GetDesiredValue(void)
			{
				ScopeGuard gaurd(m_Lock);

				return (m_DesiredValue = m_Value + 1);
			}

			void OpenGLFence::SetValue(const uint64& Value)
			{
				ScopeGuard gaurd(m_Lock);

				m_Value = Value;

				if (m_Value >= m_DesiredValue)
					PlatformOS::WaitForSignalEvent(m_Event);
			}
		}
	}
}