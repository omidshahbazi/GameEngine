// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INTERNAL_RENDER_TARGETS_H
#define INTERNAL_RENDER_TARGETS_H

#include <MemoryManagement\Singleton.h>
#include <RenderCommon\Enums.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace RenderCommon;
	using namespace Containers;

	namespace RenderSystem
	{
		class RenderTarget;

		namespace Private
		{
			class InternalRenderTargets
			{
			private:
				typedef Map<RenderTargets, const RenderTarget*> RenderTargetMap;

				SINGLETON_DECLARATION(InternalRenderTargets);

			private:
				InternalRenderTargets(void);
				~InternalRenderTargets(void);

			public:
				void Bind(RenderTargets RenderTargetPoint, const RenderTarget* RenderTarget);

				const RenderTarget* Get(RenderTargets RenderTarget) const;

			private:
				RenderTargetMap m_RenderTargets;
			};
		}
	}
}

#endif