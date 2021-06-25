// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <Rendering\IDevice.h>
#include <Rendering\Sprite.h>
#include <Rendering\RenderingCommon.h>
#include <Rendering\Private\ProgramConstantHolder.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		class RENDERING_API Pass : public ProgramConstantHolder
		{
		public:
			Pass(void);
			Pass(ProgramResource* Program);
			Pass(const Pass& Other);
			Pass(Pass&& Other);

			ProgramResource* GetProgram(void)
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

			INLINE IDevice::State& GetRenderState(void)
			{
				return m_RenderState;
			}

			INLINE const IDevice::State& GetRenderState(void) const
			{
				return m_RenderState;
			}

			void SetRenderState(const IDevice::State& State);

			INLINE Pass& operator=(const Pass& Other);
			INLINE Pass& operator=(Pass&& Other);

		private:
			ProgramResource* m_Program;
			RenderQueues m_Queue;
			IDevice::State m_RenderState;
		};
	}
}

#endif