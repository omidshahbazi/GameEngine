// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <RenderDevice\IDevice.h>
#include <RenderSystem\Sprite.h>
#include <RenderSystem\Private\ProgramConstantHolder.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace RenderDevice;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API Pass : public ProgramConstantHolder
		{
		public:
			Pass(void);
			Pass(ProgramResource* Program);
			Pass(const Pass& Other);
			Pass(Pass&& Other);
			~Pass(void);

			const ProgramResource* GetProgram(void) const
			{
				return m_Program;
			}

			bool SetProgram(ProgramResource* Program);

			INLINE void SetQueue(RenderQueues Queue)
			{
				m_Queue = Queue;
			}

			INLINE RenderQueues GetQueue(void) const
			{
				return m_Queue;
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

			INLINE Pass& operator=(const Pass& Other);

		private:
			ProgramResource* m_Program;
			RenderQueues m_Queue;
			RenderState m_RenderState;
		};
	}
}

#endif