// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		struct Color
		{
		public:
			Color(uint8 R, uint8 G, uint8 B)
			{
				*((int8*)(&m_Value)) = R;
				*((int8*)(&m_Value) + 1) = G;
				*((int8*)(&m_Value) + 2) = B;
				*((int8*)(&m_Value) + 3) = 255;
			}

			Color(uint8 R, uint8 G, uint8 B, uint8 A)
			{
				*((int8*)(&m_Value)) = R;
				*((int8*)(&m_Value) + 1) = G;
				*((int8*)(&m_Value) + 2) = B;
				*((int8*)(&m_Value) + 3) = A;
			}

			INLINE uint8 GetR(void) const
			{
				return *((int8*)(&m_Value));
			}

			INLINE uint8 GetG(void) const
			{
				return *((int8*)(&m_Value) + 1);
			}

			INLINE uint8 GetB(void) const
			{
				return *((int8*)(&m_Value) + 2);
			}

			INLINE uint8 GetA(void) const
			{
				return *((int8*)(&m_Value) + 3);
			}

		private:
			uint32 m_Value;
		};
	}
}

#endif