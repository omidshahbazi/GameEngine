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
		INLINE Color<float32>& operator << (Color<float32>& FloatColor, const Color<T>& Value)
		{
			FloatColor.R = Value.R / 255.0F;
			FloatColor.G = Value.G / 255.0F;
			FloatColor.B = Value.B / 255.0F;
			FloatColor.A = Value.A / 255.0F;

			return FloatColor;
		}

		template<typename T>
		INLINE Color<T>& operator << (Color<T>& FloatColor, const Color<float32>& Value)
		{
			FloatColor.R = Value.R * 255;
			FloatColor.G = Value.G * 255;
			FloatColor.B = Value.B * 255;
			FloatColor.A = Value.A * 255;

			return FloatColor;
		}

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

		typedef Color<float32> ColorF;
		typedef Color<uint8> ColorUI8;
		typedef Color<uint16> ColorUI16;
		typedef Color<uint32> ColorUI32;
	}
}

#endif