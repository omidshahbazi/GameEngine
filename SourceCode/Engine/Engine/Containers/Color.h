// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		template<typename T>
		struct Color
		{
		public:
			Color(void) :
				R(0),
				G(0),
				B(0),
				A(255)
			{
			}

			Color(T R, T G, T B) :
				R(R),
				G(G),
				B(B),
				A(255)
			{
			}

			Color(T R, T G, T B, T A) :
				R(R),
				G(G),
				B(B),
				A(A)
			{
			}

			INLINE bool operator==(const Color& Other) const
			{
				return (R == Other.R && G == Other.G && B == Other.B && A == Other.A);
			}

			INLINE bool operator!=(const Color& Other) const
			{
				return !(*this == Other);
			}

		public:
			T R;
			T G;
			T B;
			T A;

		public:
			static const Color<T> White;
			static const Color<T> Black;
			static const Color<T> Red;
			static const Color<T> Green;
			static const Color<T> Blue;
			static const Color<T> Cyan;
			static const Color<T> Magenta;
			static const Color<T> Yellow;
		};

		template<typename T>
		const Color<T> Color<T>::White = { 255, 255, 255, 255 };

		template<typename T>
		const Color<T> Color<T>::Black = { 0, 0, 0, 255 };

		template<typename T>
		const Color<T> Color<T>::Red = { 255, 0, 0, 255 };

		template<typename T>
		const Color<T> Color<T>::Green = { 0, 255, 0, 255 };

		template<typename T>
		const Color<T> Color<T>::Blue = { 0, 0, 255, 255 };

		template<typename T>
		const Color<T> Color<T>::Cyan = { 0, 255, 255, 255 };

		template<typename T>
		const Color<T> Color<T>::Magenta = { 255, 0, 255, 255 };

		template<typename T>
		const Color<T> Color<T>::Yellow = { 255, 255, 0, 255 };

		typedef Color<uint8> ColorUI8;
		typedef Color<uint16> ColorUI16;
		typedef Color<uint32> ColorUI32;
	}
}

#endif