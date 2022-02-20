// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_FENCE_H
#define OPEN_GL_FENCE_H

#include <Common\ScopeGuard.h>
#include <RenderDevice\IFence.h>
#include <Platform\PlatformOS.h>

namespace Engine
{
	using namespace Common;
	using namespace RenderDevice;
	using namespace Platform;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			class OpenGLDevice;

			class OpenGLFence : public IFence
			{
			public:
				OpenGLFence(OpenGLDevice* Device);
				~OpenGLFence(void);

				bool GetIsPassed(void) override;

				bool Wait(void) override;

				const uint64& GetDesiredValue(void);

				void SetValue(const uint64& Value);

			private:
				SpinLock m_Lock;
				uint64 m_Value;
				uint64 m_DesiredValue;
				PlatformOS::Handle m_Event;
			};
		}
	}
}

#endif