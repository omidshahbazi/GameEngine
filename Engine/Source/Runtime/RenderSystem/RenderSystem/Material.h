// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <RenderSystem\Private\ProgramInstance.h>

namespace Engine
{
	namespace RenderSystem
	{
		using namespace Private;

		class ConstantBuffer;

		class RENDERSYSTEM_API Material : public ProgramInstance
		{
			typedef ProgramBufferMap<ConstantBuffer> ProgramConstantBufferMap;

		public:
			Material(void);
			Material(ProgramResource* Program);
			Material(const Material& Other);

			INLINE RenderQueues GetQueue(void) const
			{
				return m_Queue;
			}

			INLINE void SetQueue(RenderQueues Queue)
			{
				m_Queue = Queue;
			}

			INLINE RenderState& GetRenderState(void)
			{
				return m_RenderState;
			}

			INLINE const RenderState& GetRenderState(void) const
			{
				return m_RenderState;
			}

			void SetRenderState(const RenderState& State);

			Material& operator=(const Material& Other);
			Material& operator=(Material&& Other);

		private:
			RenderQueues m_Queue;
			RenderState m_RenderState;
		};
	}
}

#endif