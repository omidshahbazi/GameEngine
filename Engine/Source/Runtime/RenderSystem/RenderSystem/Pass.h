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
			void UpdateData(ProgramResource* Program);

		private:
			ProgramResource* m_Program;
			RenderQueues m_Queue;
			IDevice::State m_RenderState;
		};
	}
}

#endif