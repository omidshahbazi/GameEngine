// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <Common\PrimitiveTypes.h>
//#include <Color.Reflection.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		//REFLECTION_STRUCT()
		struct RENDERING_API Color
		{
			//COLOR_OBJECT()

		public:
			Color() :
				Color(0, 0, 0, 255)
			{
			}

			Color(uint8 R, uint8 G, uint8 B) :
				Color(R, G, B, 255)
			{
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

			INLINE float32 GetFloat32R(void) const
			{
				return GetR() / 255.0F;
			}

			INLINE float32 GetFloat32G(void) const
			{
				return GetG() / 255.0F;
			}

			INLINE float32 GetFloat32B(void) const
			{
				return GetB() / 255.0F;
			}

			INLINE float32 GetFloat32A(void) const
			{
				return GetA() / 255.0F;
			}

			INLINE bool operator==(Color Other) const
			{
				return (m_Value == Other.m_Value);
			}

			INLINE bool operator!=(Color Other) const
			{
				return !(m_Value == Other.m_Value);
			}

		private:
			uint32 m_Value;

		public:
			static const Color White;
			static const Color Black;
			static const Color Red;
			static const Color Green;
			static const Color Blue;
			static const Color Cyan;
			static const Color Magenta;
			static const Color Yellow;
		};
	}
}

#endif