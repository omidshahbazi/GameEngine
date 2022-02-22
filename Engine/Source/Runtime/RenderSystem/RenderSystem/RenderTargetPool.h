// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_TARGET_POOL_H
#define RENDER_TARGET_POOL_H

#include <MemoryManagement\Singleton.h>
#include <RenderDevice\RenderTargetInfo.h>
#include <Containers\Map.h>
#include <Containers\Stack.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderSystem
	{
		using namespace RenderDevice;

		class DeviceInterface;
		class RenderTarget;

		class RENDERSYSTEM_API RenderTargetPool
		{
			friend class DeviceInterface;

		private:
			typedef Stack<RenderTarget*> RenderTargetStack;
			typedef Map<uint32, RenderTargetStack> RenderTargetMap;

			SINGLETON_DECLARATION(RenderTargetPool)

		private:
			RenderTargetPool(void);
			~RenderTargetPool(void);

			void Initialize(DeviceInterface* Device);

		public:
			RenderTarget* Get(const RenderTargetInfo* Info);
			void Back(RenderTarget* RenderTarget);

		private:
			DeviceInterface* m_Device;
			RenderTargetMap m_RenderTargets;
		};
	}
}

#endif