// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <Rendering\Pass.h>
#include <Rendering\RenderingCommon.h>

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API Material
		{
		public:
			typedef Vector<Pass> PassList;

		public:
			Material(void) :
				m_Queue(RenderQueues::Default)
			{
			}

			INLINE void AddPass(const Pass &Pass)
			{
				m_Passes.Add(Pass);
			}

			INLINE void RemovePass(uint8 Index)
			{
				m_Passes.RemoveAt(Index);
			}

			INLINE PassList &GetPasses(void)
			{
				return m_Passes;
			}

			INLINE const PassList &GetPasses(void) const
			{
				return m_Passes;
			}

			INLINE void SetQueue(RenderQueues Queue)
			{
				m_Queue = Queue;
			}

			INLINE RenderQueues GetQueue(void) const
			{
				return m_Queue;
			}

		private:
			PassList m_Passes;
			RenderQueues m_Queue;
		};
	}
}

#endif