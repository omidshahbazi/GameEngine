// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <Rendering\Pass.h>

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API Material
		{
		public:
			typedef Vector<Pass> PassList;

		public:
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

		private:
			PassList m_Passes;
		};
	}
}

#endif