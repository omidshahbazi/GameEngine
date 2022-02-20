// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\InternalRenderTargets.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		namespace Private
		{
			SINGLETON_DEFINITION(InternalRenderTargets);

			InternalRenderTargets::InternalRenderTargets(void) :
				m_RenderTargets(RenderSystemAllocators::ContainersAllocator)
			{
				Bind(RenderTargets::Context, nullptr);
			}

			InternalRenderTargets::~InternalRenderTargets(void)
			{
				m_RenderTargets.Clear();
			}

			void InternalRenderTargets::Bind(RenderTargets RenderTargetPoint, const RenderTarget* RenderTarget)
			{
				m_RenderTargets[RenderTargetPoint] = RenderTarget;
			}

			const RenderTarget* InternalRenderTargets::Get(RenderTargets RenderTarget) const
			{
				return m_RenderTargets[RenderTarget];
			}
		}
	}
}