// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <Rendering\Pass.h>

namespace Engine
{
	namespace Rendering
	{
		//TODO: Copy material
		class RENDERING_API Material
		{
		public:
			typedef Vector<Pass> PassList;

		public:
			Material(void)
			{
			}

			Material(const Material& Other)
			{
				*this = Other;
			}

			INLINE void AddPass(const Pass& Pass)
			{
				m_Passes.Add(Pass);
			}

			INLINE void RemovePass(uint8 Index)
			{
				m_Passes.RemoveAt(Index);
			}

			INLINE PassList& GetPasses(void)
			{
				return m_Passes;
			}

			INLINE const PassList& GetPasses(void) const
			{
				return m_Passes;
			}

			INLINE Material& operator = (const Material& Other)
			{
				m_Passes = Other.m_Passes;

				return *this;
			}

		private:
			PassList m_Passes;
		};
	}
}

#endif