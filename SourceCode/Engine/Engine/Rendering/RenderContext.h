// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		class RENDERING_API RenderContext
		{
			friend class DeviceInterface;

		public:
#if X64
			typedef uint64 Handle;
#else
			typedef uint32 Handle;
#endif

		private:
			RenderContext(Handle Handle) :
				m_Handle(Handle)
			{
			}

		private:
			Handle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			Handle m_Handle;
		};
	}
}

#endif